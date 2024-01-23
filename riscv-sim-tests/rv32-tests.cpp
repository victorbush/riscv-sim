#include <array>
#include <gtest/gtest.h>
#include <map>

#include "rv32.h"
#include "test-utils.h"

using namespace riscv_sim;
using namespace std;

TEST(decode_rv32i_itype, VariousTests) {

	uint32_t instruction = (321 << 20) | (2 << 15) | (3 << 12) | (5 << 7) | (0b0010011);
	auto result = Rv32_decoder::decode_itype(instruction);
	EXPECT_EQ(result.opcode, Rv32i_opcode::op_imm);
	EXPECT_EQ(result.funct3, 3);
	EXPECT_EQ(result.rd, Rv_register_id::x5);
	EXPECT_EQ(result.rs1, Rv_register_id::x2);
	EXPECT_EQ(result.imm.get_signed(), 321);
}

TEST(decode_rv32i_itype, EnsureImmSignExtended) {
	// Immediates are sign extended

	// Immediate with 0 value
	uint32_t instruction = (0b0000'0000'0000 << 20) | (2 << 15) | (3 << 12) | (5 << 7) | (0b0010011);
	auto result = Rv32_decoder::decode_itype(instruction);
	EXPECT_EQ(result.imm.get_signed(), 0);

	// Immediate with non-zero value, but 0 in sign bit
	instruction = (0b0101'0000'0000 << 20) | (2 << 15) | (3 << 12) | (5 << 7) | (0b0010011);
	result = Rv32_decoder::decode_itype(instruction);
	EXPECT_EQ(result.imm.get_signed(), 1280);

	// Immediate with non-zero value, but 1 in sign bit
	instruction = (0b1101'0000'0000 << 20) | (2 << 15) | (3 << 12) | (5 << 7) | (0b0010011);
	result = Rv32_decoder::decode_itype(instruction);
	EXPECT_EQ(result.imm.get_signed(), -768);

	// Immediate with all 12 bits set
	instruction = (0b1111'1111'1111 << 20) | (2 << 15) | (3 << 12) | (5 << 7) | (0b0010011);
	result = Rv32_decoder::decode_itype(instruction);
	EXPECT_EQ(result.imm.get_signed(), -1);
}

TEST(decode_instruction_type, SLTI) {

	auto instruction = Rv32_encoder::encode_slti(Rv_register_id::x1, Rv_register_id::x2, 123);
	auto type = Rv32_decoder::decode_instruction_type(instruction);
	EXPECT_EQ(type, Rv32i_instruction_type::slti);
}

TEST(encode_btype, ValidInstruction) {

	auto instruction = Rv32_encoder::encode_btype(Rv32i_opcode::branch, Rv32_branch_funct3::bge, Rv_register_id::x2, Rv_register_id::x15, Rv_btype_imm::from_offset(-320));
	auto result = Rv32_decoder::decode_btype(instruction);
	EXPECT_EQ(result.opcode, Rv32i_opcode::branch);
	EXPECT_EQ(result.funct3, to_underlying(Rv32_branch_funct3::bge));
	EXPECT_EQ(result.rs1, Rv_register_id::x2);
	EXPECT_EQ(result.rs2, Rv_register_id::x15);
	EXPECT_EQ(result.imm.get_offset(), -320);
}

TEST(encode_addi, ValidInstruction) {

	auto instruction = Rv32_encoder::encode_addi(Rv_register_id::x2, Rv_register_id::x9, 123);
	auto result = Rv32_decoder::decode_itype(instruction);
	EXPECT_EQ(result.opcode, Rv32i_opcode::op_imm);
	EXPECT_EQ(result.funct3, to_underlying(Rv32_op_imm_funct::addi));
	EXPECT_EQ(result.rd, Rv_register_id::x2);
	EXPECT_EQ(result.rs1, Rv_register_id::x9);
	EXPECT_EQ(result.imm.get_signed(), 123);
}

TEST(encode_auipc, ValidInstruction) {

	auto instruction = Rv32_encoder::encode_auipc(Rv_register_id::x2, 0b1111'1111'1111'1111'1111'1111);
	auto result = Rv32_decoder::decode_utype(instruction);
	EXPECT_EQ(result.opcode, Rv32i_opcode::auipc);
	EXPECT_EQ(result.rd, Rv_register_id::x2);

	// Ensure only 20 bits used for immediate
	EXPECT_EQ(result.imm.get_decoded(), 0b1111'1111'1111'1111'1111'0000'0000'0000);
}

TEST(encode_lui, ValidInstruction) {

	auto instruction = Rv32_encoder::encode_lui(Rv_register_id::x2, 0b1111'1111'1111'1111'1111'1111);
	auto result = Rv32_decoder::decode_utype(instruction);
	EXPECT_EQ(result.opcode, Rv32i_opcode::lui);
	EXPECT_EQ(result.rd, Rv_register_id::x2);

	// Ensure only 20 bits used for immediate
	EXPECT_EQ(result.imm.get_decoded(), 0b1111'1111'1111'1111'1111'0000'0000'0000);
}

TEST(encode_slli, ValidInstruction) {

	// Set 6 bits in shift_amount and then verify that only 5 bits are used.
	auto instruction = Rv32_encoder::encode_slli(Rv_register_id::x2, Rv_register_id::x9, 0b111111);
	auto result = Rv32_decoder::decode_itype(instruction);
	EXPECT_EQ(result.opcode, Rv32i_opcode::op_imm);
	EXPECT_EQ(result.funct3, to_underlying(Rv32_op_imm_funct::slli));
	EXPECT_EQ(result.rd, Rv_register_id::x2);
	EXPECT_EQ(result.rs1, Rv_register_id::x9);
	EXPECT_EQ(result.imm.get_signed(), 0b11111);
}

TEST(encode_slti, ValidInstruction) {

	auto instruction = Rv32_encoder::encode_slti(Rv_register_id::x2, Rv_register_id::x9, 123);
	auto result = Rv32_decoder::decode_itype(instruction);
	EXPECT_EQ(result.opcode, Rv32i_opcode::op_imm);
	EXPECT_EQ(result.funct3, to_underlying(Rv32_op_imm_funct::slti));
	EXPECT_EQ(result.rd, Rv_register_id::x2);
	EXPECT_EQ(result.rs1, Rv_register_id::x9);
	EXPECT_EQ(result.imm.get_signed(), 123);
}

TEST(encode_srai, ValidInstruction) {

	// Set 6 bits in shift_amount and then verify that only 5 bits are used.
	auto instruction = Rv32_encoder::encode_srai(Rv_register_id::x2, Rv_register_id::x9, 0b111111);
	auto result = Rv32_decoder::decode_itype(instruction);
	EXPECT_EQ(result.opcode, Rv32i_opcode::op_imm);
	EXPECT_EQ(result.funct3, to_underlying(Rv32_op_imm_funct::srxi));
	EXPECT_EQ(result.rd, Rv_register_id::x2);
	EXPECT_EQ(result.rs1, Rv_register_id::x9);
	EXPECT_EQ(result.imm.get_signed(), 0b0100'0001'1111);
}

TEST(encode_srli, ValidInstruction) {

	// Set 6 bits in shift_amount and then verify that only 5 bits are used.
	auto instruction = Rv32_encoder::encode_srli(Rv_register_id::x2, Rv_register_id::x9, 0b111111);
	auto result = Rv32_decoder::decode_itype(instruction);
	EXPECT_EQ(result.opcode, Rv32i_opcode::op_imm);
	EXPECT_EQ(result.funct3, to_underlying(Rv32_op_imm_funct::srxi));
	EXPECT_EQ(result.rd, Rv_register_id::x2);
	EXPECT_EQ(result.rs1, Rv_register_id::x9);
	EXPECT_EQ(result.imm.get_signed(), 0b11111);
}

TEST(get_register_id, ValidRange) {

	auto result = Rv32_decoder::get_register_id(0);
	EXPECT_EQ(result, Rv_register_id::x0);

	result = Rv32_decoder::get_register_id(31);
	EXPECT_EQ(result, Rv_register_id::x31);
}

TEST(get_register_id, InvalidRange) {

	// Upper bits are ignored

	auto result = Rv32_decoder::get_register_id(32);
	EXPECT_EQ(result, Rv_register_id::x0);

	result = Rv32_decoder::get_register_id(0b11100111);
	EXPECT_EQ(result, Rv_register_id::x7);

	result = Rv32_decoder::get_register_id(0xFF);
	EXPECT_EQ(result, Rv_register_id::x31);
}

TEST(get_opcode, ValidAndInvalidValues) {

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
		auto result = Rv32_decoder::get_opcode(test.first);
		EXPECT_EQ(result, test.second);
	}
}

/* ========================================================

Rv_btype_imm

======================================================== */

// List of test bit pairs.
// (encoded instruction bit index, decoded offset bit index)
const array<pair<uint8_t, uint8_t>, 12> btype_test_bits = {
	make_pair(7, 11),
	make_pair(8, 1),
	make_pair(9, 2),
	make_pair(10, 3),
	make_pair(11, 4),
	make_pair(25, 5),
	make_pair(26, 6),
	make_pair(27, 7),
	make_pair(28, 8),
	make_pair(29, 9),
	make_pair(30, 10),
	make_pair(31, 12),
};

TEST(Rv_btype_imm, get_offset) {

	// The instruction bit index is set.
	// Then the B-type immediate is created.
	// Then the offset is verified to see if the expected offset bit is set.

	for (const auto& pair : btype_test_bits) {
		uint32_t instruction = 1 << pair.first;
		auto imm = Rv_btype_imm::from_instruction(instruction);
		
		// If the last bit is set, expect sign extend
		if (pair.second == 12)
			EXPECT_EQ(0b1111'1111'1111'1111'1111'0000'0000'0000, imm.get_offset());
		else
			EXPECT_EQ(1 << pair.second, imm.get_offset());
	}
}

TEST(Rv_btype_imm, get_encoded) {

	// The offset bit index is set.
	// Then the B-type immediate is created.
	// Then the encoded instruction is verified to see if the expected bit is set.

	for (const auto& pair : btype_test_bits) {
		int32_t offset = 1 << pair.second;

		// If the last bit is set, expect sign extend
		if (pair.second == 12)
			offset |= 0b1111'1111'1111'1111'1111'0000'0000'0000;

		auto imm = Rv_btype_imm::from_offset(offset);
		EXPECT_EQ(1 << pair.first, imm.get_encoded());
	}
}

TEST(Rv_btype_imm, NotAMultipleOf2) {

	// Offsets are only multiples of 2
	EXPECT_THROW_EX(Rv_btype_imm::from_offset(-3), "Conditional branch offsets must be multiples of 2.");
}

TEST(Rv_btype_imm, AtMinValue) {

	auto imm = Rv_btype_imm::from_offset(-4096);
	EXPECT_EQ(imm.get_offset(), -4096);
}

TEST(Rv_btype_imm, BelowMinValue) {

	EXPECT_THROW_EX(Rv_btype_imm::from_offset(-4098), "Conditional branch offsets must fall in the range [-4096, 4094].");
}

TEST(Rv_btype_imm, AtMaxValue) {

	auto imm = Rv_btype_imm::from_offset(4094);
	EXPECT_EQ(imm.get_offset(), 4094);
}

TEST(Rv_btype_imm, AboveMaxValue) {

	EXPECT_THROW_EX(Rv_btype_imm::from_offset(4096), "Conditional branch offsets must fall in the range [-4096, 4094].");
}

/* ========================================================

Rv_itype_imm

======================================================== */

TEST(Rv_itype_imm, from_instruction__ValidNegative) {

	auto instruction = Rv32_encoder::encode_lb(Rv_register_id::x1, Rv_register_id::x2, -1);
	auto imm = Rv_itype_imm::from_instruction(instruction);
	EXPECT_EQ(imm.get_encoded(), -1 << 20);
	EXPECT_EQ(imm.get_signed(), -1);
	EXPECT_EQ(imm.get_unsigned(), 0xFFF);
}

TEST(Rv_itype_imm, from_instruction__ValidPositive) {

	auto instruction = Rv32_encoder::encode_lb(Rv_register_id::x1, Rv_register_id::x2, 27);
	auto imm = Rv_itype_imm::from_instruction(instruction);
	EXPECT_EQ(imm.get_encoded(), 27 << 20);
	EXPECT_EQ(imm.get_signed(), 27);
	EXPECT_EQ(imm.get_unsigned(), 27);
}

TEST(Rv_itype_imm, from_signed_AtMaxValue) {

	auto imm = Rv_itype_imm::from_signed(2047);
	EXPECT_EQ(imm.get_encoded(), 2047 << 20);
	EXPECT_EQ(imm.get_signed(), 2047);
	EXPECT_EQ(imm.get_unsigned(), 2047);
}

TEST(Rv_itype_imm, from_signed_AtMinValue) {

	auto imm = Rv_itype_imm::from_signed(-2048);
	EXPECT_EQ(imm.get_encoded(), -2048 << 20);
	EXPECT_EQ(imm.get_signed(), -2048);
	EXPECT_EQ(imm.get_unsigned(), 2048);
}

TEST(Rv_itype_imm, from_signed__AboveMaxValue) {

	EXPECT_THROW_EX(Rv_itype_imm::from_signed(2048), "Signed I-immediates must fall in the range [-2048, 2047].");
}

TEST(Rv_itype_imm, from_signed__BelowMinValue) {

	EXPECT_THROW_EX(Rv_itype_imm::from_signed(-2049), "Signed I-immediates must fall in the range [-2048, 2047].");
}

TEST(Rv_itype_imm, from_unsigned_AtMaxValue) {

	auto imm = Rv_itype_imm::from_unsigned(4095);
	EXPECT_EQ(imm.get_encoded(), 4095 << 20);
	EXPECT_EQ(imm.get_signed(), -1);
	EXPECT_EQ(imm.get_unsigned(), 4095);
}

TEST(Rv_itype_imm, from_unsigned_AtMinValue) {

	auto imm = Rv_itype_imm::from_unsigned(0);
	EXPECT_EQ(imm.get_encoded(), 0);
	EXPECT_EQ(imm.get_signed(), 0);
	EXPECT_EQ(imm.get_unsigned(), 0);
}

TEST(Rv_itype_imm, from_unsigned__AboveMaxValue) {

	EXPECT_THROW_EX(Rv_itype_imm::from_unsigned(4096), "Unsigned I-immediates must fall in the range [0, 4095].");
}

/* ========================================================

Rv_jtype_imm

======================================================== */

// List of test bit pairs.
// (encoded instruction bit index, decoded offset bit index)
const array<pair<uint8_t, uint8_t>, 20> jtype_test_bits = {
	make_pair(12, 12),
	make_pair(13, 13),
	make_pair(14, 14),
	make_pair(15, 15),
	make_pair(16, 16),
	make_pair(17, 17),
	make_pair(18, 18),
	make_pair(19, 19),
	make_pair(20, 11),
	make_pair(21, 1),
	make_pair(22, 2),
	make_pair(23, 3),
	make_pair(24, 4),
	make_pair(25, 5),
	make_pair(26, 6),
	make_pair(27, 7),
	make_pair(28, 8),
	make_pair(29, 9),
	make_pair(30, 10),
	make_pair(31, 20),
};

TEST(Rv_jtype_imm, get_offset) {

	// The instruction bit index is set.
	// Then the J-type immediate is created.
	// Then the offset is verified to see if the expected offset bit is set.

	for (const auto& pair : jtype_test_bits) {
		uint32_t instruction = 1 << pair.first;
		auto imm = Rv_jtype_imm::from_instruction(instruction);

		// If the last bit is set, expect sign extend
		if (pair.second == 20)
			EXPECT_EQ(0b1111'1111'1111'0000'0000'0000'0000'0000, imm.get_offset());
		else
			EXPECT_EQ(1 << pair.second, imm.get_offset());
	}
}

TEST(Rv_jtype_imm, get_encoded) {

	// The offset bit index is set.
	// Then the J-type immediate is created.
	// Then the encoded instruction is verified to see if the expected bit is set.

	for (const auto& pair : jtype_test_bits) {
		int32_t offset = 1 << pair.second;

		// If the last bit is set, expect sign extend
		if (pair.second == 20)
			offset |= 0b1111'1111'1111'0000'0000'0000'0000'0000;

		auto imm = Rv_jtype_imm::from_offset(offset);
		EXPECT_EQ(1 << pair.first, imm.get_encoded());
	}
}

TEST(Rv_jtype_imm, NotAMultipleOf2) {

	// Offsets are only multiples of 2
	EXPECT_THROW_EX(Rv_jtype_imm::from_offset(-3), "J-type offsets must be multiples of 2.");
}

TEST(Rv_jtype_imm, AtMinValue) {

	auto imm = Rv_jtype_imm::from_offset(-1048576);
	EXPECT_EQ(imm.get_offset(), -1048576);
}

TEST(Rv_jtype_imm, BelowMinValue) {

	EXPECT_THROW_EX(Rv_jtype_imm::from_offset(-1048578), "J-type offsets must fall in the range [-1048576, 1048574].");
}

TEST(Rv_jtype_imm, AtMaxValue) {

	auto imm = Rv_jtype_imm::from_offset(1048574);
	EXPECT_EQ(imm.get_offset(), 1048574);
}

TEST(Rv_jtype_imm, AboveMaxValue) {

	EXPECT_THROW_EX(Rv_jtype_imm::from_offset(1048576), "J-type offsets must fall in the range [-1048576, 1048574].");
}

/* ========================================================

Rv_stype_imm

======================================================== */

TEST(Rv_stype_imm, from_instruction__Valid) {

	auto instruction = Rv32_encoder::encode_sb(Rv_register_id::x1, Rv_register_id::x2, -1);
	auto imm = Rv_stype_imm::from_instruction(instruction);
	EXPECT_EQ(imm.get_encoded(), 0b1111'1110'0000'0000'0000'1111'1000'0000);
	EXPECT_EQ(imm.get_offset(), -1);
}

TEST(Rv_stype_imm, from_instruction__SignExtended) {

	// Ensure sign extended
	uint32_t instruction = 1 << 31;
	auto imm = Rv_stype_imm::from_instruction(instruction);
	EXPECT_EQ(imm.get_encoded(), 1 << 31);
	EXPECT_EQ(imm.get_offset(), -1 << 11);
}

TEST(Rv_stype_imm, from_offset__AtMaxValue) {

	auto imm = Rv_stype_imm::from_offset(2047);
	EXPECT_EQ(imm.get_offset(), 2047);
}

TEST(Rv_stype_imm, from_offset_AtMinValue) {

	auto imm = Rv_stype_imm::from_offset(-2048);
	EXPECT_EQ(imm.get_offset(), -2048);
}

TEST(Rv_stype_imm, from_offset__AboveMaxValue) {

	EXPECT_THROW_EX(Rv_stype_imm::from_offset(2048), "S-immediates must fall in the range [-2048, 2047].");
}

TEST(Rv_stype_imm, from_offset__BelowMinValue) {

	EXPECT_THROW_EX(Rv_stype_imm::from_offset(-2049), "S-immediates must fall in the range [-2048, 2047].");
}
