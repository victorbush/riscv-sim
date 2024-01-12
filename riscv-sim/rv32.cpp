#include "rv32.h"

#include <array>
#include <map>
#include <vector>
#include <utility>

using namespace std;

namespace riscv_sim {

const uint32_t rv32i_rtype_mask = 0b1111111'00000'00000'111'00000'1111111;
const uint32_t rv32i_itype_mask = 0b000000000000'00000'111'00000'1111111;
const uint32_t rv32i_stype_mask = rv32i_itype_mask;
const uint32_t rv32i_btype_mask = rv32i_itype_mask;
const uint32_t rv32i_utype_mask = 0b00000000000000000000'00000'1111111;
const uint32_t rv32i_jtype_mask = rv32i_utype_mask;

static uint32_t create_rtype_signature(Rv32i_opcode opcode, uint8_t funct3, uint8_t funct7)
{
	return to_underlying(opcode) | (funct3 << 12) | (funct7 << 25);
}

static uint32_t create_itype_signature(Rv32i_opcode opcode, uint8_t funct3)
{
	return to_underlying(opcode) | (funct3 << 12);
}

static uint32_t create_stype_signature(Rv32i_opcode opcode, uint8_t funct3)
{
	return create_itype_signature(opcode, funct3);
}

static uint32_t create_btype_signature(Rv32i_opcode opcode, uint8_t funct3)
{
	return create_itype_signature(opcode, funct3);
}

static uint32_t create_utype_signature(Rv32i_opcode opcode)
{
	return to_underlying(opcode);
}

static uint32_t create_jtype_signature(Rv32i_opcode opcode)
{
	return create_utype_signature(opcode);
}

static uint32_t create_op_signature(Rv32_op_funct3 funct3, Rv32_op_funct7 funct7)
{
	return create_rtype_signature(Rv32i_opcode::op, to_underlying(funct3), to_underlying(funct7));
}

static uint32_t create_op_imm_signature(Rv32_op_imm_funct funct)
{
	return create_itype_signature(Rv32i_opcode::op_imm, to_underlying(funct));
}

/** Function type used for functions that resolve instruction type lookups. */
using Rv_instruction_type_resolver = std::add_pointer_t<Rv32i_instruction_type(uint32_t instruction)>;


static Rv32i_instruction_type resolve_op_imm_shift_right(uint32_t instruction)
{
	// SRAI (arithmetic) has bit 30 set. SRLI (logical) does not.
	const auto arithmetic_mask = (1 << 30);

	if (instruction & arithmetic_mask)
		return Rv32i_instruction_type::srai;

	return Rv32i_instruction_type::srli;
}

struct Signature_match
{
	Signature_match(Rv32i_instruction_type type) : instruction(type), resolver(nullptr)
	{
	}

	Signature_match(Rv_instruction_type_resolver resolver) : resolver(resolver), instruction(Rv32i_instruction_type::invalid)
	{
	}

	/**
	The instruction type. This is invalid if a resolver is specified.
	*/
	Rv32i_instruction_type instruction;

	/**
	Picks the correct instruction type for the current instruction. Allows differentiating instructions
	that share the same opcode and funct, but differ in some other bit(s).
	If null, no resolver is needed and a single instruction type matches the signature.
	*/
	Rv_instruction_type_resolver resolver;
};

/**
Identifies what type of instruction a raw instruction is based on the instruction's signature.
An instruction signature has fixed bits (opcode, funct3, funct7, etc.) set and all variable 
bits (rd, rs1, rs2, imm, etc.) cleared.

This map uses the signature as the key and a match as a value. A signature match can either be
a single instruction type or a pointer to a function that will pick the correct instruction type.
*/
const map<uint32_t, Signature_match> instruction_signature_map2 = {
	
	{ create_utype_signature(Rv32i_opcode::lui), Rv32i_instruction_type::lui },
	{ create_utype_signature(Rv32i_opcode::auipc), Rv32i_instruction_type::auipc },

	{ create_op_imm_signature(Rv32_op_imm_funct::addi), Rv32i_instruction_type::addi },
	{ create_op_imm_signature(Rv32_op_imm_funct::andi), Rv32i_instruction_type::andi },
	{ create_op_imm_signature(Rv32_op_imm_funct::ori), Rv32i_instruction_type::ori },
	{ create_op_imm_signature(Rv32_op_imm_funct::slli), Rv32i_instruction_type::slli },
	{ create_op_imm_signature(Rv32_op_imm_funct::slti), Rv32i_instruction_type::slti },
	{ create_op_imm_signature(Rv32_op_imm_funct::sltiu), Rv32i_instruction_type::sltiu },
	{ create_op_imm_signature(Rv32_op_imm_funct::srxi), resolve_op_imm_shift_right },
	{ create_op_imm_signature(Rv32_op_imm_funct::xori), Rv32i_instruction_type::xori },

	{ create_op_signature(Rv32_op_funct3::add, Rv32_op_funct7::add), Rv32i_instruction_type::add },
	{ create_op_signature(Rv32_op_funct3::and_, Rv32_op_funct7::and_), Rv32i_instruction_type::and_ },
	{ create_op_signature(Rv32_op_funct3::or_, Rv32_op_funct7::or_), Rv32i_instruction_type::or_ },
	{ create_op_signature(Rv32_op_funct3::sll, Rv32_op_funct7::sll), Rv32i_instruction_type::sll },
	{ create_op_signature(Rv32_op_funct3::slt, Rv32_op_funct7::slt), Rv32i_instruction_type::slt },
	{ create_op_signature(Rv32_op_funct3::sltu, Rv32_op_funct7::sltu), Rv32i_instruction_type::sltu },
	{ create_op_signature(Rv32_op_funct3::sra, Rv32_op_funct7::sra), Rv32i_instruction_type::sra },
	{ create_op_signature(Rv32_op_funct3::srl, Rv32_op_funct7::srl), Rv32i_instruction_type::srl },
	{ create_op_signature(Rv32_op_funct3::sub, Rv32_op_funct7::sub), Rv32i_instruction_type::sub },
	{ create_op_signature(Rv32_op_funct3::xor_, Rv32_op_funct7::xor_), Rv32i_instruction_type::xor_ },

};

//
//// Quick way to identify what type of an instruction a raw instruction is.
//// An instruction signature has fixed bits (opcode, funct3, funct7, etc.) set
//// and all variables bits (rd, rs1, rs2, imm, etc.) cleared.
//const map<uint32_t, Rv32i_instruction_type> instruction_signature_map = {
//	//		static auto lui = Rv32i_instruction_mask::create_utype(0b0110111);
//	//static auto auipc = Rv32i_instruction_mask::create_utype(0b0010111);
//	//static auto jal = Rv32i_instruction_mask::create_jtype(0b1101111);
//	//static auto jalr = Rv32i_instruction_mask::create_itype(0b1100111, 0b000);
//
//	//static auto beq = Rv32i_instruction_mask::create_btype(0b1100011, 0b000);
//	//static auto bne = Rv32i_instruction_mask::create_btype(0b1100011, 0b001);
//	//static auto blt = Rv32i_instruction_mask::create_btype(0b1100011, 0b100);
//	//static auto bge = Rv32i_instruction_mask::create_btype(0b1100011, 0b101);
//	//static auto bltu = Rv32i_instruction_mask::create_btype(0b1100011, 0b110);
//	//static auto bgeu = Rv32i_instruction_mask::create_btype(0b1100011, 0b111);
//
//	//static auto lb = Rv32i_instruction_mask::create_itype(0b0000011, 0b000);
//	//static auto lh = Rv32i_instruction_mask::create_itype(0b0000011, 0b001);
//	//static auto lw = Rv32i_instruction_mask::create_itype(0b0000011, 0b010);
//	//static auto lbu = Rv32i_instruction_mask::create_itype(0b0000011, 0b100);
//	//static auto lhu = Rv32i_instruction_mask::create_itype(0b0000011, 0b101);
//
//	//static auto sb = Rv32i_instruction_mask::create_stype(0b0100011, 0b000);
//	//static auto sh = Rv32i_instruction_mask::create_stype(0b0100011, 0b001);
//	//static auto sw = Rv32i_instruction_mask::create_stype(0b0100011, 0b010);
//
//		{ create_itype_signature(Rv32i_opcode::op_imm, to_underlying(Rv32_op_imm_funct::addi)), Rv32i_instruction_type::addi },
//		{ create_itype_signature(Rv32i_opcode::op_imm, to_underlying(Rv32_op_imm_funct::andi)), Rv32i_instruction_type::andi },
//		{ create_itype_signature(Rv32i_opcode::op_imm, to_underlying(Rv32_op_imm_funct::ori)), Rv32i_instruction_type::ori },
//		{ create_itype_signature(Rv32i_opcode::op_imm, to_underlying(Rv32_op_imm_funct::slti)), Rv32i_instruction_type::slti },
//		{ create_itype_signature(Rv32i_opcode::op_imm, to_underlying(Rv32_op_imm_funct::sltiu)), Rv32i_instruction_type::sltiu },
//		{ create_itype_signature(Rv32i_opcode::op_imm, to_underlying(Rv32_op_imm_funct::xori)), Rv32i_instruction_type::xori },
//
//		//static auto slti = Rv32i_instruction_mask::create_itype(0b0010011, 0b010);
//		//static auto sltiu = Rv32i_instruction_mask::create_itype(0b0010011, 0b011);
//		//static auto xori = Rv32i_instruction_mask::create_itype(0b0010011, 0b100);
//		//static auto ori = Rv32i_instruction_mask::create_itype(0b0010011, 0b110);
//		//static auto andi = Rv32i_instruction_mask::create_itype(0b0010011, 0b111);
//
//		//static auto slli = Rv32i_instruction_mask::create_rtype(0b0010011, 0b001, 0);
//		//static auto srli = Rv32i_instruction_mask::create_rtype(0b0010011, 0b101, 0);
//		//static auto srai = Rv32i_instruction_mask::create_rtype(0b0010011, 0b101, 0b0100000);
//
//		//static auto add = Rv32i_instruction_mask::create_rtype(0b0110011, 0b000, 0);
//		//static auto sub = Rv32i_instruction_mask::create_rtype(0b0110011, 0b000, 0b0100000);
//		//static auto sll = Rv32i_instruction_mask::create_rtype(0b0110011, 0b001, 0);
//		//static auto slt = Rv32i_instruction_mask::create_rtype(0b0110011, 0b010, 0);
//		//static auto sltu = Rv32i_instruction_mask::create_rtype(0b0110011, 0b011, 0);
//		//static auto xor_ = Rv32i_instruction_mask::create_rtype(0b0110011, 0b100, 0);
//		//static auto srl = Rv32i_instruction_mask::create_rtype(0b0110011, 0b101, 0);
//		//static auto sra = Rv32i_instruction_mask::create_rtype(0b0110011, 0b101, 0b0100000);
//		//static auto or_ = Rv32i_instruction_mask::create_rtype(0b0110011, 0b110, 0);
//		//static auto and_ = Rv32i_instruction_mask::create_rtype(0b0110011, 0b111, 0);
//
//		//static auto fence = Rv32i_instruction_mask(0b000000000000'00000'111'00000'1111111, 0b0001111);
//		//static auto ecall = Rv32i_instruction_mask(0b111111111111'11111'111'11111'1111111, 0b000000000000'00000'000'00000'1110011);
//		//static auto ebreak = Rv32i_instruction_mask(0b111111111111'11111'111'11111'1111111, 0b000000000001'00000'000'00000'1110011);
//
//};

/**
Maps the raw opcode bits to the correct opcode enum.
*/
const auto rv32i_opcode_map = map<uint8_t, Rv32i_opcode>() = {
	{ to_underlying(Rv32i_opcode::auipc), Rv32i_opcode::auipc },
	{ to_underlying(Rv32i_opcode::branch), Rv32i_opcode::branch },
	{ to_underlying(Rv32i_opcode::jal), Rv32i_opcode::jal },
	{ to_underlying(Rv32i_opcode::jalr), Rv32i_opcode::jalr },
	{ to_underlying(Rv32i_opcode::load), Rv32i_opcode::load },
	{ to_underlying(Rv32i_opcode::lui), Rv32i_opcode::lui },
	{ to_underlying(Rv32i_opcode::misc_mem), Rv32i_opcode::misc_mem },
	{ to_underlying(Rv32i_opcode::op), Rv32i_opcode::op },
	{ to_underlying(Rv32i_opcode::op_imm), Rv32i_opcode::op_imm },
	{ to_underlying(Rv32i_opcode::store), Rv32i_opcode::store },
	{ to_underlying(Rv32i_opcode::system), Rv32i_opcode::system },
};

/**
Maps the raw opcode bits to the correct instruction mask to use.
An instruction mask is applied to the raw instruction to get its signature.

signature = instruction & mask
*/
const auto rv32_opcode_mask_map = map<uint8_t, uint32_t>() = {
	{ to_underlying(Rv32i_opcode::auipc), rv32i_utype_mask },
	//{ to_underlying(Rv32i_opcode::branch), rv32i_btype_mask },
	//{ to_underlying(Rv32i_opcode::jal), rv32i_jtype_mask },
	//{ to_underlying(Rv32i_opcode::jalr), rv32i_itype_mask },
	//{ to_underlying(Rv32i_opcode::load), rv32i_itype_mask },
	{ to_underlying(Rv32i_opcode::lui), rv32i_utype_mask },
	//{ to_underlying(Rv32i_opcode::misc_mem), rv32i_itype_mask },
	{ to_underlying(Rv32i_opcode::op), rv32i_rtype_mask },
	{ to_underlying(Rv32i_opcode::op_imm), rv32i_itype_mask },
	//{ to_underlying(Rv32i_opcode::store), rv32i_stype_mask },
	//{ to_underlying(Rv32i_opcode::system), rv32i_itype_mask },
};

Rv32i_instruction_type Rv32i_decoder::decode_rv32i_instruction_type(uint32_t instruction)
{
	// Opcode is in first 7 bits
	uint8_t opcode_raw = instruction & 0b1111111;
	if (!rv32_opcode_mask_map.contains(opcode_raw))
		return Rv32i_instruction_type::invalid;

	// Mask off fixed bits based on the opcode of the instruction to get a unique signature
	auto mask = rv32_opcode_mask_map.at(opcode_raw);
	auto sig = instruction & mask;

	// Lookup the instruction type based on the signature
	if (instruction_signature_map2.contains(sig))
	{
		const auto v = instruction_signature_map2.at(sig);
		if (v.resolver != nullptr)
			return v.resolver(instruction);

		return v.instruction;
	}

	return Rv32i_instruction_type::invalid;
}

Rv32i_itype_instruction Rv32i_decoder::decode_rv32i_itype(uint32_t instruction)
{
	// 31     20 | 19   15 | 14  12 | 11   7 | 6    0
	// imm[11:0]     rs1     funct3      rd    opcode

	const auto opcode = get_rv32i_opcode(instruction);
	if (opcode == Rv32i_opcode::invalid)
		throw exception("Invalid instruction.");

	const uint8_t rd_raw = 0b1'1111 & (instruction >> 7);
	const auto rd = get_rv32_register_id(rd_raw);

	const uint8_t funct3 = 0b111 & (instruction >> 12);

	const uint8_t rs1_raw = 0b1'1111 & (instruction >> 15);
	const auto rs1 = get_rv32_register_id(rs1_raw);

	auto imm = Rv_itype_imm(0b1111'1111'1111 & (instruction >> 20));

	return Rv32i_itype_instruction(opcode, funct3, rd, rs1, imm);
}

Rv_rtype_instruction Rv32i_decoder::decode_rtype(uint32_t instruction)
{
	// 31        25 | 24     20 | 19     15 | 14    12 | 11     7 | 6      0
	//    funct7         rs2         rs1       funct3        rd      opcode

	const auto opcode = get_rv32i_opcode(instruction);
	if (opcode == Rv32i_opcode::invalid)
		throw exception("Invalid instruction.");

	const uint8_t funct7 = 0b111'1111 & (instruction >> 25);

	const uint8_t rs2_raw = 0b1'1111 & (instruction >> 20);
	const auto rs2 = get_rv32_register_id(rs2_raw);

	const uint8_t rs1_raw = 0b1'1111 & (instruction >> 15);
	const auto rs1 = get_rv32_register_id(rs1_raw);

	const uint8_t funct3 = 0b111 & (instruction >> 12);

	const uint8_t rd_raw = 0b1'1111 & (instruction >> 7);
	const auto rd = get_rv32_register_id(rd_raw);

	auto i = Rv_rtype_instruction();
	i.funct3 = funct3;
	i.funct7 = funct7;
	i.rd = rd;
	i.rs1 = rs1;
	i.rs2 = rs2;

	return i;
}

Rv_utype_instruction Rv32i_decoder::decode_utype(uint32_t instruction)
{
	// 31                           12 | 11   7 | 6    0
	//           imm[31:12]                 rd    opcode

	const auto opcode = get_rv32i_opcode(instruction);
	if (opcode == Rv32i_opcode::invalid)
		throw exception("Invalid instruction.");

	const uint8_t rd_raw = 0b1'1111 & (instruction >> 7);
	const auto rd = get_rv32_register_id(rd_raw);

	auto imm = Rv_utype_imm(instruction >> 12);

	return Rv_utype_instruction(opcode, rd, imm);
}

uint32_t Rv32_encoder::encode_utype(Rv32i_opcode opcode, Rv32_register_id rd, uint32_t imm)
{
	uint32_t val = (0b1111'1111'1111'1111'1111 & imm) << 12;
	return val | (to_underlying(rd) << 7) | to_underlying(opcode);
}

uint32_t Rv32_encoder::encode_op(Rv32_op_funct3 funct3, Rv32_op_funct7 funct7, Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	return (to_underlying(funct7) << 25) | (to_underlying(rs2) << 20) | (to_underlying(rs1) << 15) | (to_underlying(funct3) << 12) | (to_underlying(rd) << 7) | (to_underlying(Rv32i_opcode::op));
}

uint32_t Rv32_encoder::encode_op_imm(Rv32_op_imm_funct funct, Rv32_register_id rd, Rv32_register_id rs1, uint16_t imm)
{
	// 12-bit immediate, ignore other bits
	imm &= 0b1111'1111'1111;
	return (imm << 20) | (to_underlying(rs1) << 15) | (to_underlying(funct) << 12) | (to_underlying(rd) << 7) | (to_underlying(Rv32i_opcode::op_imm));
}





uint32_t Rv32_encoder::encode_add(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	return encode_op(Rv32_op_funct3::add, Rv32_op_funct7::add, rd, rs1, rs2);
}

uint32_t Rv32_encoder::encode_addi(Rv32_register_id rd, Rv32_register_id rs1, uint16_t imm)
{
	return encode_op_imm(Rv32_op_imm_funct::addi, rd, rs1, imm);
}

uint32_t Rv32_encoder::encode_and(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	return encode_op(Rv32_op_funct3::and_, Rv32_op_funct7::and_, rd, rs1, rs2);
}

uint32_t Rv32_encoder::encode_andi(Rv32_register_id rd, Rv32_register_id rs1, uint16_t imm)
{
	return encode_op_imm(Rv32_op_imm_funct::andi, rd, rs1, imm);
}

uint32_t Rv32_encoder::encode_auipc(Rv32_register_id rd, uint32_t imm)
{
	return encode_utype(Rv32i_opcode::auipc, rd, imm);
}

uint32_t Rv32_encoder::encode_lui(Rv32_register_id rd, uint32_t imm)
{
	return encode_utype(Rv32i_opcode::lui, rd, imm);
}

uint32_t Rv32_encoder::encode_or(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	return encode_op(Rv32_op_funct3::or_, Rv32_op_funct7::or_, rd, rs1, rs2);
}

uint32_t Rv32_encoder::encode_ori(Rv32_register_id rd, Rv32_register_id rs1, uint16_t imm)
{
	return encode_op_imm(Rv32_op_imm_funct::ori, rd, rs1, imm);
}

uint32_t Rv32_encoder::encode_sll(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	return encode_op(Rv32_op_funct3::sll, Rv32_op_funct7::sll, rd, rs1, rs2);
}

uint32_t Rv32_encoder::encode_slli(Rv32_register_id rd, Rv32_register_id rs1, uint8_t shift_amount)
{
	return encode_op_imm(Rv32_op_imm_funct::slli, rd, rs1, 0b11111 & shift_amount);
}

uint32_t Rv32_encoder::encode_slt(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	return encode_op(Rv32_op_funct3::slt, Rv32_op_funct7::slt, rd, rs1, rs2);
}

uint32_t Rv32_encoder::encode_slti(Rv32_register_id rd, Rv32_register_id rs1, uint16_t imm)
{
	return encode_op_imm(Rv32_op_imm_funct::slti, rd, rs1, imm);
}

uint32_t Rv32_encoder::encode_sltiu(Rv32_register_id rd, Rv32_register_id rs1, uint16_t imm)
{
	return encode_op_imm(Rv32_op_imm_funct::sltiu, rd, rs1, imm);
}

uint32_t Rv32_encoder::encode_sra(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	return encode_op(Rv32_op_funct3::sra, Rv32_op_funct7::sra, rd, rs1, rs2);
}

uint32_t Rv32_encoder::encode_sltu(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	return encode_op(Rv32_op_funct3::sltu, Rv32_op_funct7::sltu, rd, rs1, rs2);
}

uint32_t Rv32_encoder::encode_srai(Rv32_register_id rd, Rv32_register_id rs1, uint8_t shift_amount)
{
	return encode_op_imm(Rv32_op_imm_funct::srxi, rd, rs1, (1 << 10) | (0b11111 & shift_amount));
}

uint32_t Rv32_encoder::encode_srl(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	return encode_op(Rv32_op_funct3::srl, Rv32_op_funct7::srl, rd, rs1, rs2);
}

uint32_t Rv32_encoder::encode_srli(Rv32_register_id rd, Rv32_register_id rs1, uint8_t shift_amount)
{
	return encode_op_imm(Rv32_op_imm_funct::srxi, rd, rs1, 0b11111 & shift_amount);
}

uint32_t Rv32_encoder::encode_sub(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	return encode_op(Rv32_op_funct3::sub, Rv32_op_funct7::sub, rd, rs1, rs2);
}

uint32_t Rv32_encoder::encode_xori(Rv32_register_id rd, Rv32_register_id rs1, uint16_t imm)
{
	return encode_op_imm(Rv32_op_imm_funct::xori, rd, rs1, imm);
}

uint32_t Rv32_encoder::encode_xor(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	return encode_op(Rv32_op_funct3::xor_, Rv32_op_funct7::xor_, rd, rs1, rs2);
}

Rv32_register_id Rv32i_decoder::get_rv32_register_id(uint8_t encoded_register)
{
	// Register encoding uses 5 bits for a total of 32 possible values.
	// The high three bits are ignored here.
	return Rv32_register_id(static_cast<Rv32_register_id>(0b11111 & encoded_register));
}

Rv32i_opcode Rv32i_decoder::get_rv32i_opcode(uint32_t instruction)
{
	const auto opcode_raw = 0b1111111 & instruction;

	if (rv32i_opcode_map.contains(opcode_raw))
		return rv32i_opcode_map.at(opcode_raw);

	return Rv32i_opcode::invalid;
}

Rv_itype_imm::Rv_itype_imm(uint16_t val)
{
	// 12-bit immediate, ignore other bits
	value = val & 0b1111'1111'1111;

	// 12-bit immediate is sign extended. If bit 11 is 1, set remaining high bits to 1.
	if (value & 0b1000'0000'0000)
		value |= (0b1111'1111'1111'1111'1111'0000'0000'0000);
}

int32_t Rv_itype_imm::to_i32() const
{
	return static_cast<int32_t>(value);
}

uint32_t Rv_itype_imm::to_u32() const
{
	return value;
}

uint8_t Rv_itype_imm::get_shift_amount() const
{
	return static_cast<uint8_t>(0b11111 & value);
}

Rv_utype_imm::Rv_utype_imm(uint32_t encoded)
{
	_encoded = encoded;
	_decoded = (encoded & 0b1111'1111'1111'1111'1111) << 12;
}

uint32_t Rv_utype_imm::get_decoded() const
{
	return _decoded;
}

Rv_btype_imm::Rv_btype_imm(uint8_t encoded_7to11, uint8_t encoded_25to31)
	: _encoded_7to11(encoded_7to11), _encoded_25to31(encoded_25to31)
{
	/*
	Instruction bit:  31 30 29 28 27 26 25    11 10 9 8  7
	Offset bit:       12 10  9  8  7  6  5     4  3 2 1 11
	*/

	const auto bit11 = (_encoded_7to11 & 1) << 11;
	const auto bit1to4 = _encoded_7to11 & 0b11110;
	const auto bit5to10 = (_encoded_25to31 & 0b111111) << 5;
	const auto bit12 = (_encoded_25to31 & 0b1000000) << 6;

	_offset = bit1to4 | bit5to10 | bit11 | bit12;

	// Sign extend
	if (bit12)
		_offset |= 0b1111'1111'1111'1111'1111'0000'0000'0000;
}

int32_t Rv_btype_imm::get_offset() const
{
	return _offset;
}

}
