#include <gtest/gtest.h>
#include <map>

#include "rv32.h"

using namespace riscv_sim;
using namespace std;

TEST(decode_rv32i_itype, VariousTests) {

	uint32_t instruction = (321 << 20) | (2 << 15) | (3 << 12) | (5 << 7) | (0b0010011);
	auto result = Rv32i_decoder::decode_rv32i_itype(instruction);
	EXPECT_EQ(result.opcode, Rv32i_opcode::op_imm);
	EXPECT_EQ(result.funct3, 3);
	EXPECT_EQ(result.rd, Rv32_register_id::x5);
	EXPECT_EQ(result.rs1, Rv32_register_id::x2);
	EXPECT_EQ(result.imm.to_i32(), 321);
}

TEST(decode_rv32i_itype, EnsureImmSignExtended) {
	// Immediates are sign extended

	// Immediate with 0 value
	uint32_t instruction = (0b0000'0000'0000 << 20) | (2 << 15) | (3 << 12) | (5 << 7) | (0b0010011);
	auto result = Rv32i_decoder::decode_rv32i_itype(instruction);
	EXPECT_EQ(result.imm.to_i32(), 0);

	// Immediate with non-zero value, but 0 in sign bit
	instruction = (0b0101'0000'0000 << 20) | (2 << 15) | (3 << 12) | (5 << 7) | (0b0010011);
	result = Rv32i_decoder::decode_rv32i_itype(instruction);
	EXPECT_EQ(result.imm.to_i32(), 1280);

	// Immediate with non-zero value, but 1 in sign bit
	instruction = (0b1101'0000'0000 << 20) | (2 << 15) | (3 << 12) | (5 << 7) | (0b0010011);
	result = Rv32i_decoder::decode_rv32i_itype(instruction);
	EXPECT_EQ(result.imm.to_i32(), -768);

	// Immediate with all 12 bits set
	instruction = (0b1111'1111'1111 << 20) | (2 << 15) | (3 << 12) | (5 << 7) | (0b0010011);
	result = Rv32i_decoder::decode_rv32i_itype(instruction);
	EXPECT_EQ(result.imm.to_i32(), -1);
}

TEST(decode_rv32i_instruction_type, SLTI) {

	auto instruction = Rv32_encoder::encode_slti(Rv32_register_id::x1, Rv32_register_id::x2, 123);
	auto type = Rv32i_decoder::decode_rv32i_instruction_type(instruction);
	EXPECT_EQ(type, Rv32i_instruction_type::slti);
}

TEST(encode_addi, ValidInstruction) {

	auto instruction = Rv32_encoder::encode_addi(Rv32_register_id::x2, Rv32_register_id::x9, 123);
	auto result = Rv32i_decoder::decode_rv32i_itype(instruction);
	EXPECT_EQ(result.opcode, Rv32i_opcode::op_imm);
	EXPECT_EQ(result.funct3, to_underlying(Rv32_op_imm_funct::addi));
	EXPECT_EQ(result.rd, Rv32_register_id::x2);
	EXPECT_EQ(result.rs1, Rv32_register_id::x9);
	EXPECT_EQ(result.imm.to_i32(), 123);
}

TEST(encode_auipc, ValidInstruction) {

	auto instruction = Rv32_encoder::encode_auipc(Rv32_register_id::x2, 0b1111'1111'1111'1111'1111'1111);
	auto result = Rv32i_decoder::decode_utype(instruction);
	EXPECT_EQ(result.opcode, Rv32i_opcode::auipc);
	EXPECT_EQ(result.rd, Rv32_register_id::x2);

	// Ensure only 20 bits used for immediate
	EXPECT_EQ(result.imm.get_decoded(), 0b1111'1111'1111'1111'1111'0000'0000'0000);
}

TEST(encode_lui, ValidInstruction) {

	auto instruction = Rv32_encoder::encode_lui(Rv32_register_id::x2, 0b1111'1111'1111'1111'1111'1111);
	auto result = Rv32i_decoder::decode_utype(instruction);
	EXPECT_EQ(result.opcode, Rv32i_opcode::lui);
	EXPECT_EQ(result.rd, Rv32_register_id::x2);

	// Ensure only 20 bits used for immediate
	EXPECT_EQ(result.imm.get_decoded(), 0b1111'1111'1111'1111'1111'0000'0000'0000);
}

TEST(encode_slli, ValidInstruction) {

	// Set 6 bits in shift_amount and then verify that only 5 bits are used.
	auto instruction = Rv32_encoder::encode_slli(Rv32_register_id::x2, Rv32_register_id::x9, 0b111111);
	auto result = Rv32i_decoder::decode_rv32i_itype(instruction);
	EXPECT_EQ(result.opcode, Rv32i_opcode::op_imm);
	EXPECT_EQ(result.funct3, to_underlying(Rv32_op_imm_funct::slli));
	EXPECT_EQ(result.rd, Rv32_register_id::x2);
	EXPECT_EQ(result.rs1, Rv32_register_id::x9);
	EXPECT_EQ(result.imm.to_u32(), 0b11111);
}

TEST(encode_slti, ValidInstruction) {

	auto instruction = Rv32_encoder::encode_slti(Rv32_register_id::x2, Rv32_register_id::x9, 123);
	auto result = Rv32i_decoder::decode_rv32i_itype(instruction);
	EXPECT_EQ(result.opcode, Rv32i_opcode::op_imm);
	EXPECT_EQ(result.funct3, to_underlying(Rv32_op_imm_funct::slti));
	EXPECT_EQ(result.rd, Rv32_register_id::x2);
	EXPECT_EQ(result.rs1, Rv32_register_id::x9);
	EXPECT_EQ(result.imm.to_i32(), 123);
}

TEST(encode_srai, ValidInstruction) {

	// Set 6 bits in shift_amount and then verify that only 5 bits are used.
	auto instruction = Rv32_encoder::encode_srai(Rv32_register_id::x2, Rv32_register_id::x9, 0b111111);
	auto result = Rv32i_decoder::decode_rv32i_itype(instruction);
	EXPECT_EQ(result.opcode, Rv32i_opcode::op_imm);
	EXPECT_EQ(result.funct3, to_underlying(Rv32_op_imm_funct::srxi));
	EXPECT_EQ(result.rd, Rv32_register_id::x2);
	EXPECT_EQ(result.rs1, Rv32_register_id::x9);
	EXPECT_EQ(result.imm.to_u32(), 0b0100'0001'1111);
}

TEST(encode_srli, ValidInstruction) {

	// Set 6 bits in shift_amount and then verify that only 5 bits are used.
	auto instruction = Rv32_encoder::encode_srli(Rv32_register_id::x2, Rv32_register_id::x9, 0b111111);
	auto result = Rv32i_decoder::decode_rv32i_itype(instruction);
	EXPECT_EQ(result.opcode, Rv32i_opcode::op_imm);
	EXPECT_EQ(result.funct3, to_underlying(Rv32_op_imm_funct::srxi));
	EXPECT_EQ(result.rd, Rv32_register_id::x2);
	EXPECT_EQ(result.rs1, Rv32_register_id::x9);
	EXPECT_EQ(result.imm.to_u32(), 0b11111);
}

TEST(get_rv32_register_id, ValidRange) {

	auto result = Rv32i_decoder::get_rv32_register_id(0);
	EXPECT_EQ(result, Rv32_register_id::x0);

	result = Rv32i_decoder::get_rv32_register_id(31);
	EXPECT_EQ(result, Rv32_register_id::x31);
}

TEST(get_rv32_register_id, InvalidRange) {

	// Upper bits are ignored

	auto result = Rv32i_decoder::get_rv32_register_id(32);
	EXPECT_EQ(result, Rv32_register_id::x0);

	result = Rv32i_decoder::get_rv32_register_id(0b11100111);
	EXPECT_EQ(result, Rv32_register_id::x7);

	result = Rv32i_decoder::get_rv32_register_id(0xFF);
	EXPECT_EQ(result, Rv32_register_id::x31);
}

TEST(get_rv32i_opcode, ValidAndInvalidValues) {

	// Map of input instruction value => expected opcode result
	const auto test_cases = std::map<uint32_t, Rv32i_opcode>() = {

		// Opcode only tests
		{ 0b0110011, Rv32i_opcode::op },
		{ 0b0010011, Rv32i_opcode::op_imm },
		{ 0b0110111, Rv32i_opcode::lui },
		{ 0b0010111, Rv32i_opcode::auipc },
		{ 0b1101111, Rv32i_opcode::jal },
		{ 0b1100111, Rv32i_opcode::jalr },
		{ 0b1100011, Rv32i_opcode::branch },
		{ 0b0000011, Rv32i_opcode::load },
		{ 0b0100011, Rv32i_opcode::store },
		{ 0b0001111, Rv32i_opcode::misc_mem },
		{ 0b1110011, Rv32i_opcode::system },

		// Full instruction tests
		{ 0x00430313, Rv32i_opcode::op_imm },

		// Invalid tests
		{ 0, Rv32i_opcode::invalid },
		{ 0xFFFFFFFF, Rv32i_opcode::invalid },
		{ 0x123321FF, Rv32i_opcode::invalid },
		{ 0x12332100, Rv32i_opcode::invalid },
	};

	for (const auto test : test_cases) {
		auto result = Rv32i_decoder::get_rv32i_opcode(test.first);
		EXPECT_EQ(result, test.second);
	}
}
