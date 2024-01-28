#include "rv32.h"

#include <array>
#include <map>
#include <stdexcept>
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

static uint32_t create_rtype_signature(Rv_opcode opcode, uint8_t funct3, uint8_t funct7)
{
	return to_underlying(opcode) | (funct3 << 12) | (funct7 << 25);
}

static uint32_t create_itype_signature(Rv_opcode opcode, uint8_t funct3)
{
	return to_underlying(opcode) | (funct3 << 12);
}

static uint32_t create_stype_signature(Rv_opcode opcode, uint8_t funct3)
{
	return create_itype_signature(opcode, funct3);
}

static uint32_t create_btype_signature(Rv_opcode opcode, uint8_t funct3)
{
	return create_itype_signature(opcode, funct3);
}

static uint32_t create_utype_signature(Rv_opcode opcode)
{
	return to_underlying(opcode);
}

static uint32_t create_jtype_signature(Rv_opcode opcode)
{
	return create_utype_signature(opcode);
}

static uint32_t create_branch_signature(Rv32_branch_funct3 funct3)
{
	return create_btype_signature(Rv_opcode::branch, to_underlying(funct3));
}

static uint32_t create_load_signature(Rv32_load_funct3 funct3)
{
	return create_itype_signature(Rv_opcode::load, to_underlying(funct3));
}

static uint32_t create_miscmem_signature(Rv32_miscmem_funct3 funct3)
{
	return create_itype_signature(Rv_opcode::misc_mem, to_underlying(funct3));
}

static uint32_t create_op_signature(Rv32_op_funct3 funct3, Rv32_op_funct7 funct7)
{
	return create_rtype_signature(Rv_opcode::op, to_underlying(funct3), to_underlying(funct7));
}

static uint32_t create_op_imm_signature(Rv32_op_imm_funct funct)
{
	return create_itype_signature(Rv_opcode::op_imm, to_underlying(funct));
}

static uint32_t create_store_signature(Rv32_store_funct3 funct3)
{
	return create_stype_signature(Rv_opcode::store, to_underlying(funct3));
}

static uint32_t create_system_signature(Rv32_system_funct3 funct3)
{
	return create_itype_signature(Rv_opcode::system, to_underlying(funct3));
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

static Rv32i_instruction_type resolve_system_priv(uint32_t instruction)
{
	// Type of instruction is immediate in the I-type immediate value

	const auto imm = instruction >> 20;
	if (imm == to_underlying(Rv32_system_funct12::ebreak))
		return Rv32i_instruction_type::ebreak;
	if (imm == to_underlying(Rv32_system_funct12::ecall))
		return Rv32i_instruction_type::ecall;

	return Rv32i_instruction_type::invalid;
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
	
	{ create_utype_signature(Rv_opcode::auipc), Rv32i_instruction_type::auipc },
	{ create_utype_signature(Rv_opcode::lui), Rv32i_instruction_type::lui },

	{ create_jtype_signature(Rv_opcode::jal), Rv32i_instruction_type::jal },
	{ create_jtype_signature(Rv_opcode::jalr), Rv32i_instruction_type::jalr },

	{ create_branch_signature(Rv32_branch_funct3::beq), Rv32i_instruction_type::beq },
	{ create_branch_signature(Rv32_branch_funct3::bge), Rv32i_instruction_type::bge },
	{ create_branch_signature(Rv32_branch_funct3::bgeu), Rv32i_instruction_type::bgeu },
	{ create_branch_signature(Rv32_branch_funct3::blt), Rv32i_instruction_type::blt },
	{ create_branch_signature(Rv32_branch_funct3::bltu), Rv32i_instruction_type::bltu },
	{ create_branch_signature(Rv32_branch_funct3::bne), Rv32i_instruction_type::bne },

	{ create_op_imm_signature(Rv32_op_imm_funct::addi), Rv32i_instruction_type::addi },
	{ create_op_imm_signature(Rv32_op_imm_funct::andi), Rv32i_instruction_type::andi },
	{ create_op_imm_signature(Rv32_op_imm_funct::ori), Rv32i_instruction_type::ori },
	{ create_op_imm_signature(Rv32_op_imm_funct::slli), Rv32i_instruction_type::slli },
	{ create_op_imm_signature(Rv32_op_imm_funct::slti), Rv32i_instruction_type::slti },
	{ create_op_imm_signature(Rv32_op_imm_funct::sltiu), Rv32i_instruction_type::sltiu },
	{ create_op_imm_signature(Rv32_op_imm_funct::srxi), resolve_op_imm_shift_right },
	{ create_op_imm_signature(Rv32_op_imm_funct::xori), Rv32i_instruction_type::xori },

	{ create_load_signature(Rv32_load_funct3::lb), Rv32i_instruction_type::lb },
	{ create_load_signature(Rv32_load_funct3::lbu), Rv32i_instruction_type::lbu },
	{ create_load_signature(Rv32_load_funct3::lh), Rv32i_instruction_type::lh },
	{ create_load_signature(Rv32_load_funct3::lhu), Rv32i_instruction_type::lhu },
	{ create_load_signature(Rv32_load_funct3::lw), Rv32i_instruction_type::lw },

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

	{ create_store_signature(Rv32_store_funct3::sb), Rv32i_instruction_type::sb },
	{ create_store_signature(Rv32_store_funct3::sh), Rv32i_instruction_type::sh },
	{ create_store_signature(Rv32_store_funct3::sw), Rv32i_instruction_type::sw },

	{ create_miscmem_signature(Rv32_miscmem_funct3::fence), Rv32i_instruction_type::fence },

	{ create_system_signature(Rv32_system_funct3::priv), resolve_system_priv },
};

/**
Maps the raw opcode bits to the correct opcode enum.
*/
const auto rv32i_opcode_map = map<uint8_t, Rv_opcode>() = {
	{ to_underlying(Rv_opcode::auipc), Rv_opcode::auipc },
	{ to_underlying(Rv_opcode::branch), Rv_opcode::branch },
	{ to_underlying(Rv_opcode::jal), Rv_opcode::jal },
	{ to_underlying(Rv_opcode::jalr), Rv_opcode::jalr },
	{ to_underlying(Rv_opcode::load), Rv_opcode::load },
	{ to_underlying(Rv_opcode::lui), Rv_opcode::lui },
	{ to_underlying(Rv_opcode::misc_mem), Rv_opcode::misc_mem },
	{ to_underlying(Rv_opcode::op), Rv_opcode::op },
	{ to_underlying(Rv_opcode::op_imm), Rv_opcode::op_imm },
	{ to_underlying(Rv_opcode::store), Rv_opcode::store },
	{ to_underlying(Rv_opcode::system), Rv_opcode::system },
};

/**
Maps the raw opcode bits to the correct instruction mask to use.
An instruction mask is applied to the raw instruction to get its signature.

signature = instruction & mask
*/
const auto rv32_opcode_mask_map = map<uint8_t, uint32_t>() = {
	{ to_underlying(Rv_opcode::auipc), rv32i_utype_mask },
	{ to_underlying(Rv_opcode::branch), rv32i_btype_mask },
	{ to_underlying(Rv_opcode::jal), rv32i_jtype_mask },
	{ to_underlying(Rv_opcode::jalr), rv32i_itype_mask },
	{ to_underlying(Rv_opcode::load), rv32i_itype_mask },
	{ to_underlying(Rv_opcode::lui), rv32i_utype_mask },
	{ to_underlying(Rv_opcode::misc_mem), rv32i_itype_mask },
	{ to_underlying(Rv_opcode::op), rv32i_rtype_mask },
	{ to_underlying(Rv_opcode::op_imm), rv32i_itype_mask },
	{ to_underlying(Rv_opcode::store), rv32i_stype_mask },
	{ to_underlying(Rv_opcode::system), rv32i_itype_mask },
};

/* ========================================================

Rv32_decoder

======================================================== */

Rv32i_instruction_type Rv32_decoder::decode_instruction_type(uint32_t instruction)
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

Rv_btype_instruction Rv32_decoder::decode_btype(uint32_t instruction)
{
	// 31        25 | 24     20 | 19     15 | 14    12 | 11     7 | 6      0
	// imm[12|10:5]      rs2         rs1       funct3   imm[4:1|11]  opcode

	const auto opcode = get_opcode(instruction);
	if (opcode == Rv_opcode::invalid)
		throw runtime_error("Invalid instruction.");

	const uint8_t funct3 = 0b111 & (instruction >> 12);

	const auto imm = Rv_btype_imm::from_instruction(instruction);

	const uint8_t rs2_raw = 0b1'1111 & (instruction >> 20);
	const auto rs2 = get_register_id(rs2_raw);

	const uint8_t rs1_raw = 0b1'1111 & (instruction >> 15);
	const auto rs1 = get_register_id(rs1_raw);

	return Rv_btype_instruction(opcode, funct3, rs1, rs2, imm);
}

Rv_itype_instruction Rv32_decoder::decode_itype(uint32_t instruction)
{
	// 31     20 | 19   15 | 14  12 | 11   7 | 6    0
	// imm[11:0]     rs1     funct3      rd    opcode

	const auto opcode = get_opcode(instruction);
	if (opcode == Rv_opcode::invalid)
		throw runtime_error("Invalid instruction.");

	const uint8_t rd_raw = 0b1'1111 & (instruction >> 7);
	const auto rd = get_register_id(rd_raw);

	const uint8_t funct3 = 0b111 & (instruction >> 12);

	const uint8_t rs1_raw = 0b1'1111 & (instruction >> 15);
	const auto rs1 = get_register_id(rs1_raw);

	auto imm = Rv_itype_imm::from_instruction(instruction);

	return Rv_itype_instruction(opcode, funct3, rd, rs1, imm);
}

Rv_jtype_instruction Rv32_decoder::decode_jtype(uint32_t instruction)
{
	//   31    | 30         21 |   20    | 19          12 | 11    7 | 6      0
	// imm[20]     imm[10:1]     imm[11]     imm[19:12]        rd      opcode

	const auto opcode = get_opcode(instruction);
	if (opcode == Rv_opcode::invalid)
		throw runtime_error("Invalid instruction.");

	const uint8_t rd_raw = 0b1'1111 & (instruction >> 7);
	const auto rd = get_register_id(rd_raw);

	const auto imm = Rv_jtype_imm::from_instruction(instruction);

	return Rv_jtype_instruction(opcode, rd, imm);
}

Rv_rtype_instruction Rv32_decoder::decode_rtype(uint32_t instruction)
{
	// 31        25 | 24     20 | 19     15 | 14    12 | 11     7 | 6      0
	//    funct7         rs2         rs1       funct3        rd      opcode

	const auto opcode = get_opcode(instruction);
	if (opcode == Rv_opcode::invalid)
		throw runtime_error("Invalid instruction.");

	const uint8_t funct7 = 0b111'1111 & (instruction >> 25);

	const uint8_t rs2_raw = 0b1'1111 & (instruction >> 20);
	const auto rs2 = get_register_id(rs2_raw);

	const uint8_t rs1_raw = 0b1'1111 & (instruction >> 15);
	const auto rs1 = get_register_id(rs1_raw);

	const uint8_t funct3 = 0b111 & (instruction >> 12);

	const uint8_t rd_raw = 0b1'1111 & (instruction >> 7);
	const auto rd = get_register_id(rd_raw);

	return Rv_rtype_instruction(opcode, funct3, funct7, rd, rs1, rs2);
}

Rv_stype_instruction Rv32_decoder::decode_stype(uint32_t instruction)
{
	// 31        25 | 24     20 | 19     15 | 14    12 | 11     7 | 6      0
	//   imm[11:5]       rs2         rs1       funct3    imm[4:0]    opcode

	const auto opcode = get_opcode(instruction);
	if (opcode == Rv_opcode::invalid)
		throw runtime_error("Invalid instruction.");

	const auto imm = Rv_stype_imm::from_instruction(instruction);

	const uint8_t rs2_raw = 0b1'1111 & (instruction >> 20);
	const auto rs2 = get_register_id(rs2_raw);

	const uint8_t rs1_raw = 0b1'1111 & (instruction >> 15);
	const auto rs1 = get_register_id(rs1_raw);

	const uint8_t funct3 = 0b111 & (instruction >> 12);

	return Rv_stype_instruction(opcode, funct3, rs1, rs2, imm);
}

Rv_utype_instruction Rv32_decoder::decode_utype(uint32_t instruction)
{
	// 31                           12 | 11   7 | 6    0
	//           imm[31:12]                 rd    opcode

	const auto opcode = get_opcode(instruction);
	if (opcode == Rv_opcode::invalid)
		throw runtime_error("Invalid instruction.");

	const uint8_t rd_raw = 0b1'1111 & (instruction >> 7);
	const auto rd = get_register_id(rd_raw);

	auto imm = Rv_utype_imm::from_instruction(instruction);

	return Rv_utype_instruction(opcode, rd, imm);
}

Rv_register_id Rv32_decoder::get_register_id(uint8_t encoded_register)
{
	// Register encoding uses 5 bits for a total of 32 possible values.
	// The high three bits are ignored here.
	return Rv_register_id(static_cast<Rv_register_id>(0b11111 & encoded_register));
}

Rv_opcode Rv32_decoder::get_opcode(uint32_t instruction)
{
	const auto opcode_raw = 0b1111111 & instruction;

	if (rv32i_opcode_map.contains(opcode_raw))
		return rv32i_opcode_map.at(opcode_raw);

	return Rv_opcode::invalid;
}

/* ========================================================

Rv32_encoder

======================================================== */

uint32_t Rv32_encoder::encode_btype(Rv_opcode opcode, Rv32_branch_funct3 funct3, Rv_register_id rs1, Rv_register_id rs2, Rv_btype_imm imm)
{
	return imm.get_encoded() | to_underlying(opcode) | to_underlying(funct3) << 12 | to_underlying(rs1) << 15 | to_underlying(rs2) << 20;
}

static uint32_t encode_itype(Rv_opcode opcode, uint8_t funct3, Rv_register_id rs1, Rv_register_id rd, Rv_itype_imm imm)
{
	return imm.get_encoded() | (to_underlying(rs1) << 15) | ((funct3 & 0b111) << 12) | (to_underlying(rd) << 7) | (to_underlying(opcode));
}

uint32_t Rv32_encoder::encode_jal(Rv_register_id rd, Rv_jtype_imm imm)
{
	return imm.get_encoded() | (to_underlying(rd) << 7) | to_underlying(Rv_opcode::jal);
}

uint32_t Rv32_encoder::encode_jalr(Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm)
{
	return imm.get_encoded() | (to_underlying(rs1) << 15) | (to_underlying(Rv32_jalr_funct3::jalr) << 12) | (to_underlying(rd) << 7) | (to_underlying(Rv_opcode::jalr));
}

uint32_t Rv32_encoder::encode_load(Rv32_load_funct3 funct3, Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm)
{
	return imm.get_encoded() | (to_underlying(rs1) << 15) | (to_underlying(funct3) << 12) | (to_underlying(rd) << 7) | (to_underlying(Rv_opcode::load));
}

uint32_t Rv32_encoder::encode_miscmem(Rv32_miscmem_funct3 funct3, Rv_register_id rs1, Rv_register_id rd, Rv_itype_imm imm)
{
	return encode_itype(Rv_opcode::misc_mem, to_underlying(funct3), rs1, rd, imm);
}

uint32_t Rv32_encoder::encode_utype(Rv_opcode opcode, Rv_register_id rd, uint32_t imm)
{
	uint32_t val = (0b1111'1111'1111'1111'1111 & imm) << 12;
	return val | (to_underlying(rd) << 7) | to_underlying(opcode);
}

uint32_t Rv32_encoder::encode_op(Rv32_op_funct3 funct3, Rv32_op_funct7 funct7, Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2)
{
	return (to_underlying(funct7) << 25) | (to_underlying(rs2) << 20) | (to_underlying(rs1) << 15) | (to_underlying(funct3) << 12) | (to_underlying(rd) << 7) | (to_underlying(Rv_opcode::op));
}

uint32_t Rv32_encoder::encode_op_imm(Rv32_op_imm_funct funct, Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm)
{
	return imm.get_encoded() | (to_underlying(rs1) << 15) | (to_underlying(funct) << 12) | (to_underlying(rd) << 7) | (to_underlying(Rv_opcode::op_imm));
}

uint32_t Rv32_encoder::encode_store(Rv32_store_funct3 funct3, Rv_register_id rs1, Rv_register_id rs2, Rv_stype_imm imm)
{
	return imm.get_encoded() | (to_underlying(rs2) << 20) | (to_underlying(rs1) << 15) | (to_underlying(funct3) << 12) | (to_underlying(Rv_opcode::store));
}

uint32_t Rv32_encoder::encode_system(Rv32_system_funct3 funct3, Rv32_system_funct12 funct12)
{
	const auto imm = Rv_itype_imm::from_unsigned(to_underlying(funct12));
	return encode_itype(Rv_opcode::system, to_underlying(funct3), Rv_register_id::x0, Rv_register_id::x0, imm);
}

/* --------------------------------------------------------
Specific instruction encoding helpers
-----------------------------------------------------------*/

uint32_t Rv32_encoder::encode_add(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2)
{
	return encode_op(Rv32_op_funct3::add, Rv32_op_funct7::add, rd, rs1, rs2);
}

uint32_t Rv32_encoder::encode_addi(Rv_register_id rd, Rv_register_id rs1, int16_t imm)
{
	const auto immediate = Rv_itype_imm::from_signed(imm);
	return encode_op_imm(Rv32_op_imm_funct::addi, rd, rs1, immediate);
}

uint32_t Rv32_encoder::encode_and(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2)
{
	return encode_op(Rv32_op_funct3::and_, Rv32_op_funct7::and_, rd, rs1, rs2);
}

uint32_t Rv32_encoder::encode_andi(Rv_register_id rd, Rv_register_id rs1, int16_t imm)
{
	const auto immediate = Rv_itype_imm::from_signed(imm);
	return encode_op_imm(Rv32_op_imm_funct::andi, rd, rs1, immediate);
}

uint32_t Rv32_encoder::encode_auipc(Rv_register_id rd, uint32_t imm)
{
	return encode_utype(Rv_opcode::auipc, rd, imm);
}

uint32_t Rv32_encoder::encode_beq(Rv_register_id rs1, Rv_register_id rs2, int16_t offset)
{
	const auto imm = Rv_btype_imm::from_offset(offset);
	return encode_btype(Rv_opcode::branch, Rv32_branch_funct3::beq, rs1, rs2, imm);
}

uint32_t Rv32_encoder::encode_bge(Rv_register_id rs1, Rv_register_id rs2, int16_t offset)
{
	const auto imm = Rv_btype_imm::from_offset(offset);
	return encode_btype(Rv_opcode::branch, Rv32_branch_funct3::bge, rs1, rs2, imm);
}

uint32_t Rv32_encoder::encode_bgeu(Rv_register_id rs1, Rv_register_id rs2, int16_t offset)
{
	const auto imm = Rv_btype_imm::from_offset(offset);
	return encode_btype(Rv_opcode::branch, Rv32_branch_funct3::bgeu, rs1, rs2, imm);
}

uint32_t Rv32_encoder::encode_blt(Rv_register_id rs1, Rv_register_id rs2, int16_t offset)
{
	const auto imm = Rv_btype_imm::from_offset(offset);
	return encode_btype(Rv_opcode::branch, Rv32_branch_funct3::blt, rs1, rs2, imm);
}

uint32_t Rv32_encoder::encode_bltu(Rv_register_id rs1, Rv_register_id rs2, int16_t offset)
{
	const auto imm = Rv_btype_imm::from_offset(offset);
	return encode_btype(Rv_opcode::branch, Rv32_branch_funct3::bltu, rs1, rs2, imm);
}

uint32_t Rv32_encoder::encode_bne(Rv_register_id rs1, Rv_register_id rs2, int16_t offset)
{
	const auto imm = Rv_btype_imm::from_offset(offset);
	return encode_btype(Rv_opcode::branch, Rv32_branch_funct3::bne, rs1, rs2, imm);
}

uint32_t Rv32_encoder::encode_ebreak()
{
	return encode_system(Rv32_system_funct3::priv, Rv32_system_funct12::ebreak);
}

uint32_t Rv32_encoder::encode_ecall()
{
	return encode_system(Rv32_system_funct3::priv, Rv32_system_funct12::ecall);
}

uint32_t Rv32_encoder::encode_fence(Rv_register_id rs1, Rv_register_id rd, Rv_itype_imm imm)
{
	return encode_miscmem(Rv32_miscmem_funct3::fence, rs1, rd, imm);
}

uint32_t Rv32_encoder::encode_lb(Rv_register_id rd, Rv_register_id rs1, int16_t offset)
{
	const auto imm = Rv_itype_imm::from_signed(offset);
	return encode_load(Rv32_load_funct3::lb, rd, rs1, imm);
}

uint32_t Rv32_encoder::encode_lbu(Rv_register_id rd, Rv_register_id rs1, int16_t offset)
{
	const auto imm = Rv_itype_imm::from_signed(offset);
	return encode_load(Rv32_load_funct3::lbu, rd, rs1, imm);
}

uint32_t Rv32_encoder::encode_lh(Rv_register_id rd, Rv_register_id rs1, int16_t offset)
{
	const auto imm = Rv_itype_imm::from_signed(offset);
	return encode_load(Rv32_load_funct3::lh, rd, rs1, imm);
}

uint32_t Rv32_encoder::encode_lhu(Rv_register_id rd, Rv_register_id rs1, int16_t offset)
{
	const auto imm = Rv_itype_imm::from_signed(offset);
	return encode_load(Rv32_load_funct3::lhu, rd, rs1, imm);
}

uint32_t Rv32_encoder::encode_lw(Rv_register_id rd, Rv_register_id rs1, int16_t offset)
{
	const auto imm = Rv_itype_imm::from_signed(offset);
	return encode_load(Rv32_load_funct3::lw, rd, rs1, imm);
}

uint32_t Rv32_encoder::encode_lui(Rv_register_id rd, uint32_t imm)
{
	return encode_utype(Rv_opcode::lui, rd, imm);
}

uint32_t Rv32_encoder::encode_or(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2)
{
	return encode_op(Rv32_op_funct3::or_, Rv32_op_funct7::or_, rd, rs1, rs2);
}

uint32_t Rv32_encoder::encode_ori(Rv_register_id rd, Rv_register_id rs1, int16_t imm)
{
	const auto immediate = Rv_itype_imm::from_signed(imm);
	return encode_op_imm(Rv32_op_imm_funct::ori, rd, rs1, immediate);
}

uint32_t Rv32_encoder::encode_sll(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2)
{
	return encode_op(Rv32_op_funct3::sll, Rv32_op_funct7::sll, rd, rs1, rs2);
}

uint32_t Rv32_encoder::encode_slli(Rv_register_id rd, Rv_register_id rs1, uint8_t shift_amount)
{
	const auto imm = Rv_itype_imm::from_unsigned(0b11111 & shift_amount);
	return encode_op_imm(Rv32_op_imm_funct::slli, rd, rs1, imm);
}

uint32_t Rv32_encoder::encode_slt(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2)
{
	return encode_op(Rv32_op_funct3::slt, Rv32_op_funct7::slt, rd, rs1, rs2);
}

uint32_t Rv32_encoder::encode_sltu(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2)
{
	return encode_op(Rv32_op_funct3::sltu, Rv32_op_funct7::sltu, rd, rs1, rs2);
}

uint32_t Rv32_encoder::encode_slti(Rv_register_id rd, Rv_register_id rs1, int16_t imm)
{
	const auto immediate = Rv_itype_imm::from_signed(imm);
	return encode_op_imm(Rv32_op_imm_funct::slti, rd, rs1, immediate);
}

uint32_t Rv32_encoder::encode_sltiu(Rv_register_id rd, Rv_register_id rs1, uint16_t imm)
{
	const auto immediate = Rv_itype_imm::from_unsigned(imm);
	return encode_op_imm(Rv32_op_imm_funct::sltiu, rd, rs1, immediate);
}

uint32_t Rv32_encoder::encode_sb(Rv_register_id rs1, Rv_register_id rs2, int16_t offset)
{
	const auto imm = Rv_stype_imm::from_offset(offset);
	return encode_store(Rv32_store_funct3::sb, rs1, rs2, imm);
}

uint32_t Rv32_encoder::encode_sh(Rv_register_id rs1, Rv_register_id rs2, int16_t offset)
{
	const auto imm = Rv_stype_imm::from_offset(offset);
	return encode_store(Rv32_store_funct3::sh, rs1, rs2, imm);
}

uint32_t Rv32_encoder::encode_sra(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2)
{
	return encode_op(Rv32_op_funct3::sra, Rv32_op_funct7::sra, rd, rs1, rs2);
}

uint32_t Rv32_encoder::encode_srai(Rv_register_id rd, Rv_register_id rs1, uint8_t shift_amount)
{
	// Bit 10 of the immediate is set for shift right
	const auto imm = Rv_itype_imm::from_unsigned((1 << 10) | (0b11111 & shift_amount));
	return encode_op_imm(Rv32_op_imm_funct::srxi, rd, rs1, imm);
}

uint32_t Rv32_encoder::encode_srl(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2)
{
	return encode_op(Rv32_op_funct3::srl, Rv32_op_funct7::srl, rd, rs1, rs2);
}

uint32_t Rv32_encoder::encode_srli(Rv_register_id rd, Rv_register_id rs1, uint8_t shift_amount)
{
	const auto imm = Rv_itype_imm::from_unsigned(0b11111 & shift_amount);
	return encode_op_imm(Rv32_op_imm_funct::srxi, rd, rs1, imm);
}

uint32_t Rv32_encoder::encode_sub(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2)
{
	return encode_op(Rv32_op_funct3::sub, Rv32_op_funct7::sub, rd, rs1, rs2);
}

uint32_t Rv32_encoder::encode_sw(Rv_register_id rs1, Rv_register_id rs2, int16_t offset)
{
	const auto imm = Rv_stype_imm::from_offset(offset);
	return encode_store(Rv32_store_funct3::sw, rs1, rs2, imm);
}

uint32_t Rv32_encoder::encode_xori(Rv_register_id rd, Rv_register_id rs1, int16_t imm)
{
	const auto immediate = Rv_itype_imm::from_signed(imm);
	return encode_op_imm(Rv32_op_imm_funct::xori, rd, rs1, immediate);
}

uint32_t Rv32_encoder::encode_xor(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2)
{
	return encode_op(Rv32_op_funct3::xor_, Rv32_op_funct7::xor_, rd, rs1, rs2);
}

/* ========================================================

Rv_btype_imm

======================================================== */

Rv_btype_imm Rv_btype_imm::from_instruction(uint32_t instruction)
{
	/*
	Instruction bit:  31 30 29 28 27 26 25    11 10 9 8  7
	Offset bit:       12 10  9  8  7  6  5     4  3 2 1 11

	Note: b-type immediates use 12 bits of the instruction, but use
	13 total bits once decoded. Bit 0 is always 0.
	*/

	const auto bit1to4 = ((instruction >> 8) & 0b1111) << 1;
	const auto bit5to10 = ((instruction >> 25) & 0b111111) << 5;
	const auto bit11 = ((instruction >> 7) & 1) << 11;
	const auto bit12 = ((instruction >> 31) & 1) << 12;

	int32_t offset = bit1to4 | bit5to10 | bit11 | bit12;

	// Sign extend
	if (bit12)
		offset |= 0b1111'1111'1111'1111'1111'0000'0000'0000;

	// Store the encoded immediate value (clear unrelated bits of the instruction)
	uint32_t encoded = instruction & 0b1111'1110'0000'0000'0000'1111'1000'0000;

	return Rv_btype_imm(offset, encoded);
}

Rv_btype_imm Rv_btype_imm::from_offset(int32_t offset)
{
	// Offsets must be multiples of 2
	if (offset & 1)
		throw runtime_error("Conditional branch offsets must be multiples of 2.");

	// Offsets must fall in the range [-4096, 4094]
	if (offset < -4096 || offset > 4094)
		throw runtime_error("Conditional branch offsets must fall in the range [-4096, 4094].");

	const auto inst_bit_7 = ((offset >> 11) & 1) << 7;
	const auto inst_bit_8to11 = ((offset >> 1) & 0b1111) << 8;
	const auto inst_bit_25to30 = ((offset >> 5) & 0b111111) << 25;
	const auto inst_bit_31 = ((offset >> 12) & 1) << 31;

	const uint32_t encoded = inst_bit_7 | inst_bit_8to11 | inst_bit_25to30 | inst_bit_31;

	return Rv_btype_imm(offset, encoded);
}

Rv_btype_imm::Rv_btype_imm(int32_t offset, uint32_t encoded)
	: _offset(offset), _encoded(encoded)
{
}

uint32_t Rv_btype_imm::get_encoded() const
{
	return _encoded;
}

int32_t Rv_btype_imm::get_offset() const
{
	return _offset;
}

/* ========================================================

Rv_itype_imm

======================================================== */

Rv_itype_imm Rv_itype_imm::from_instruction(uint32_t instruction)
{
	auto immediate = 0b1111'1111'1111 & (instruction >> 20);
	return Rv_itype_imm(immediate);
}

Rv_itype_imm Rv_itype_imm::from_signed(int16_t immediate)
{
	// Immediate value is 12 bits only.
	// Valid signed range: [-2048, 2047]
	if (immediate < -2048 || immediate > 2047)
		throw runtime_error("Signed I-immediates must fall in the range [-2048, 2047].");

	return Rv_itype_imm(0b1111'1111'1111 & immediate);
}

Rv_itype_imm Rv_itype_imm::from_unsigned(uint16_t immediate)
{
	// Immediate value is 12 bits only.
	// Valid unsigned range: [0, 4095]
	if (immediate > 4095)
		throw runtime_error("Unsigned I-immediates must fall in the range [0, 4095].");

	return Rv_itype_imm(0b1111'1111'1111 & immediate);
}

Rv_itype_imm::Rv_itype_imm(uint16_t imm)
	: _immediate(imm)
{
}

uint32_t Rv_itype_imm::get_encoded() const
{
	return _immediate << 20;
}

int32_t Rv_itype_imm::get_signed() const
{
	// 12-bit immediate is sign extended. If bit 11 is 1, set remaining high bits to 1.
	if (_immediate & 0b1000'0000'0000)
		return _immediate | (0b1111'1111'1111'1111'1111'0000'0000'0000);

	return _immediate;
}

uint32_t Rv_itype_imm::get_unsigned() const
{
	return _immediate;
}

uint8_t Rv_itype_imm::get_shift_amount() const
{
	return static_cast<uint8_t>(0b11111 & _immediate);
}

/* ========================================================

Rv_jtype_imm

======================================================== */

//   31    | 30         21 |   20    | 19          12 | 11    7 | 6      0
// imm[20]     imm[10:1]     imm[11]     imm[19:12]        rd      opcode

Rv_jtype_imm Rv_jtype_imm::from_instruction(uint32_t instruction)
{
	const auto decoded_bits_1to10 = (instruction >> 20) & 0b0111'1111'1110;
	const auto decoded_bits_11 = (instruction >> 9) & (1 << 11);
	const auto decoded_bits_12to19 = instruction & 0b0000'0000'0000'1111'1111'0000'0000'0000;
	const auto decoded_bits_20 = (instruction >> 11) & (1 << 20);
	int32_t offset = decoded_bits_1to10 | decoded_bits_11 | decoded_bits_12to19 | decoded_bits_20;

	// Sign extend
	if (decoded_bits_20)
		offset |= 0b1111'1111'1111'0000'0000'0000'0000'0000;

	// Get the encoded bits and clear the rest of the instruction bits
	uint32_t encoded = instruction & 0b1111'1111'1111'1111'1111'0000'0000'0000;

	return Rv_jtype_imm(offset, encoded);
}

Rv_jtype_imm Rv_jtype_imm::from_offset(int32_t offset)
{
	// Offsets must be multiples of 2
	if (offset & 1)
		throw runtime_error("J-type offsets must be multiples of 2.");

	// Offsets must fall in the range [-1048576, 1048574]
	if (offset < -1048576 || offset > 1048574)
		throw runtime_error("J-type offsets must fall in the range [-1048576, 1048574].");

	const auto encoded_bits_12to19 = (offset & 0b0000'0000'0000'1111'1111'0000'0000'0000);
	const auto encoded_bits_20 = (offset & (1 << 11)) << 9;
	const auto encoded_bits_21to30 = (offset & 0b0111'1111'1110) << 20;
	const auto encoded_bits_31 = (offset & (1 << 20)) << 11;
	uint32_t encoded = encoded_bits_12to19 | encoded_bits_20 | encoded_bits_21to30 | encoded_bits_31;

	return Rv_jtype_imm(offset, encoded);
}

Rv_jtype_imm::Rv_jtype_imm(int32_t offset, uint32_t encoded)
	: _offset(offset), _encoded(encoded)
{
}

uint32_t Rv_jtype_imm::get_encoded() const
{
	return _encoded;
}

int32_t Rv_jtype_imm::get_offset() const
{
	return _offset;
}

/* ========================================================

Rv_stype_imm

======================================================== */

Rv_stype_imm Rv_stype_imm::from_instruction(uint32_t instruction)
{
	const auto bits_0to4 = (instruction >> 7) & 0b11111;
	const auto bits_5to11 = (instruction >> 25) & 0b1111111;

	int32_t offset = bits_0to4 | (bits_5to11 << 5);

	// Sign extend
	if (instruction & (1 << 31))
		offset |= 0b1111'1111'1111'1111'1111'0000'0000'0000;

	// Store the encoded immediate value (clear unrelated bits of the instruction)
	uint32_t encoded = instruction & 0b1111'1110'0000'0000'0000'1111'1000'0000;

	return Rv_stype_imm(offset, encoded);
}

Rv_stype_imm Rv_stype_imm::from_offset(int32_t offset)
{
	// Valid signed range: [-2048, 2047]
	if (offset < -2048 || offset > 2047)
		throw runtime_error("S-immediates must fall in the range [-2048, 2047].");

	const auto encoded_bits_7to11 = offset & 0b11111;
	const auto encoded_bits_25to31 = (offset >> 5) & 0b1111111;
	uint32_t encoded = (encoded_bits_7to11 << 7) | (encoded_bits_25to31 << 25);

	return Rv_stype_imm(offset, encoded);
}

Rv_stype_imm::Rv_stype_imm(int32_t offset, uint32_t encoded)
	: _offset(offset), _encoded(encoded)
{
}

uint32_t Rv_stype_imm::get_encoded() const
{
	return _encoded;
}

int32_t Rv_stype_imm::get_offset() const
{
	return _offset;
}

/* ========================================================

Rv_utype_imm

======================================================== */

Rv_utype_imm Rv_utype_imm::from_instruction(uint32_t instruction)
{
	// Use the high 20 bits - the low 12 bits are cleared
	auto encoded = instruction & (0b1111'1111'1111'1111'1111 << 12);
	return Rv_utype_imm(encoded);
}

Rv_utype_imm Rv_utype_imm::from_decoded(uint32_t decoded)
{
	// Behaves the same as from_instruction. The low 12 bits are cleared.
	return Rv_utype_imm::from_instruction(decoded);
}

Rv_utype_imm::Rv_utype_imm(uint32_t encoded)
{
	_encoded = encoded;
}

uint32_t Rv_utype_imm::get_decoded() const
{
	// The encoded and decoded values are the same
	return _encoded;
}

}
