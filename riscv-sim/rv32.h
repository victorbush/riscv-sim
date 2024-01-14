#pragma once

#include <cstdint>

namespace riscv_sim {

/** 12-bit immediate value used by B-type instructions. */
struct Rv_btype_imm
{
	/** Extracts a B-type immediate from a B-type instruction. */
	static Rv_btype_imm from_instruction(uint32_t instruction);

	/** Creates a B-type immediate from a decoded branch offset value. The upper 20 bits of the input are discarded and the lower 12 bits used. */
	static Rv_btype_imm from_offset(int32_t offset);

	/** Encodes immediate into an empty 32-bit instruction. */
	uint32_t get_encoded() const;

	/** Gets the branch offset. The 12-bit immediate is sign extended. */
	int32_t get_offset() const;

private:
	Rv_btype_imm(int32_t offset, uint32_t encoded);

	uint32_t _encoded;  // Immediate value encoded into a 32-bit instruction
	int32_t _offset;    // Decoded and sign-extended branch offset
};

/** 12-bit immediate value used by I-type instructions. The value is sign extended to 32 bits. The 12th bit determines the sign. */
struct Rv_itype_imm
{
	Rv_itype_imm(uint16_t val);

	int32_t to_i32() const;
	uint32_t to_u32() const;

	/** For shift immediate instructions, gets the shift amount. */
	uint8_t get_shift_amount() const;

private:
	uint32_t value;
};

/** 20-bit immediate value used by J-type instructions. */
struct Rv_jtype_imm
{
	Rv_jtype_imm(uint32_t encoded);
};

/** 12-bit immediate value used by S-type instructions. */
struct Rv_stype_imm
{
	Rv_stype_imm(uint8_t encoded_7to11, uint8_t encoded_25to31);

private:
	uint8_t encoded_7to11;  // Bits 7 through 11 when encoded
	uint8_t encoded_25to31; // Bits 25 through 31 when encoded
};

/** 20-bit immediate value used by U-type instructions. These bits are used as the high 20 bits for a 32-bit value. */
struct Rv_utype_imm
{
	/**
	* Creates a u-type immediate value.
	*
	* @param encoded The encoded 20-bit immediate. The high 12 bits are ignored.
	*/
	Rv_utype_imm(uint32_t encoded);

	/**
	* Gets the decoded immediate value. The 20-bit immediate is placed in the high 20 bits of the 32-bit decoded value with the remaining bits set to 0.
	*/
	uint32_t get_decoded() const;

private:
	uint32_t _encoded;
	uint32_t _decoded;
};

enum class Rv32_register_id : uint8_t
{
	x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30, x31,
	pc,
	_count,

	// ABI register aliases
	zero = x0,
	ra = x1,     // Return address
	sp = x2,     // Stack pointer
	gp = x3,     // Global pointer
	tp = x4,     // Thread pointer
	t0 = x5,     // Temporary/alternate link register
	t1 = x6,     // Temporary 1
	t2 = x7,     // Temporary 2
	s0 = x8,     // Saved register 0
	fp = s0,     // Framer pointer
	s1 = x9,     // Saved register 1
	a0 = x10,    // Function argument 0/return value 0
	a1 = x11,    // Function argument 1/return value 1
	a2 = x12,    // Function argument 2
	a3 = x13,    // Function argument 3
	a4 = x14,    // Function argument 4
	a5 = x15,    // Function argument 5
	a6 = x16,    // Function argument 6
	a7 = x17,    // Function argument 7
	s2 = x18,    // Saved register 2
	s3 = x19,    // Saved register 3
	s4 = x20,    // Saved register 4
	s5 = x21,    // Saved register 5
	s6 = x22,    // Saved register 6
	s7 = x23,    // Saved register 7
	s8 = x24,    // Saved register 8
	s9 = x25,    // Saved register 9
	s10 = x26,   // Saved register 10
	s11 = x27,   // Saved register 11
	t3 = x28,    // Temporary 3
	t4 = x29,    // Temporary 4
	t5 = x30,    // Temporary 5
	t6 = x31,    // Temporary 6
};

enum class Rv32i_opcode : uint8_t
{
	invalid = 0,

	// Integer computation

	op = 0b0110011, // Integer register-register operation
	op_imm = 0b0010011, // Integer register-immediate operation
	lui = 0b0110111, // Load upper immediate
	auipc = 0b0010111, // Add upper immediate to PR register

	// Control transfer

	jal = 0b1101111, // Jump and link
	jalr = 0b1100111, // Jump and link register (indirect jump)
	branch = 0b1100011, // Conditional branch

	// Other

	load = 0b0000011, // Memory load
	store = 0b0100011, // Memory store
	misc_mem = 0b0001111, // Fence
	system = 0b1110011, // Environment call, breakpoint
};

enum class Rv32_branch_funct3 : uint8_t
{
	beq = 0b000,
	bne = 0b001,
	blt = 0b100,
	bge = 0b101,
	bltu = 0b110,
	bgeu = 0b111,
};

enum class Rv32_load_funct3 : uint8_t
{
	lb = 0b000,
	lh = 0b001,
	lw = 0b010,
	lbu = 0b100,
	lhu = 0b101,
};

enum class Rv32_op_funct3 : uint8_t
{
	add = 0b000,
	sub = add,
	sll = 0b001,
	slt = 0b010,
	sltu = 0b011,
	xor_ = 0b100,
	srl = 0b101,
	sra = srl,
	or_ = 0b110,
	and_ = 0b111,
};

enum class Rv32_op_funct7 : uint8_t
{
	add = 0,
	sub = 0b0100000,
	sll = 0,
	slt = 0,
	sltu = 0,
	xor_ = 0,
	srl = 0,
	sra = 0b0100000,
	or_ = 0,
	and_ = 0,
};

enum class Rv32_op_imm_funct : uint8_t
{
	addi = 0b000,
	slti = 0b010,
	sltiu = 0b011,
	xori = 0b100,
	ori = 0b110,
	andi = 0b111,
	slli = 0b001,
	srxi = 0b101, // srli and srai share the same funct, difference is bit 30
};

enum class Rv32_instruction_format
{
	btype,
	itype,
	jtype,
	rtype,
	stype,
	utype,
};

enum class Rv32i_instruction_type
{
	invalid,

	miscmem,
	system,

	lui,   // Load upper immediate
	auipc, // Add upper immediate to PR register
	jal,   // Jump and link
	jalr,  // Jump and link register (indirect jump)

	// Conditional branches

	beq,   // Branch if rs1 == rs2
	bne,   // Branch if rs1 != rs2
	blt,   // Branch if rs1 < rs2 (signed)
	bltu,  // Branch if rs1 < rs2 (unsigned)
	bge,   // Branch if rs1 >= rs2 (signed)
	bgeu,  // Branch if rs1 >= rs2 (unsigned)

	// Load

	lb,    // Load byte (8-bit)
	lh,    // Load half (16-bit)
	lw,    // Load word (32-bit)
	lbu,   // Load byte unsigned
	lhu,   // Load half unsigned

	// Save

	sb,    // Store byte
	sh,    // Store half
	sw,    // Store word

	// Op immediate

	addi,  // Add immediate

	andi,  // AND immeidate
	ori,   // OR immediate
	xori,  // XOR immediate

	slti,  // Set less-than immediate
	sltiu, // Set less-than immediate unsigned

	slli,  // Shift left logicial immediate
	srli,  // Shift right logicial immediate
	srai,  // Shift right arithmetic immediate

	// Op

	add,   // Add
	sub,   // Subtract
	sll,
	slt,
	sltu,
	xor_,
	srl,
	sra,
	or_,
	and_,
};

struct Rv_btype_instruction
{
	Rv32i_opcode opcode;
	uint8_t funct3;
	Rv32_register_id rs1;
	Rv32_register_id rs2;
	Rv_btype_imm imm;
};

struct Rv_rtype_instruction
{
	Rv32i_opcode opcode;
	uint8_t funct3;
	uint8_t funct7;
	Rv32_register_id rd;
	Rv32_register_id rs1;
	Rv32_register_id rs2;
};

struct Rv_utype_instruction
{
	Rv32i_opcode opcode;
	Rv32_register_id rd;
	Rv_utype_imm imm;
};

class Rv32i_itype_instruction
{
public:
	Rv32i_itype_instruction(
		Rv32i_opcode opcode,
		uint8_t funct3,
		Rv32_register_id rd,
		Rv32_register_id rs1,
		Rv_itype_imm imm)
		:
		opcode(opcode),
		funct3(funct3),
		rd(rd),
		rs1(rs1),
		imm(imm)
	{
	}

	Rv32i_opcode opcode;
	uint8_t funct3; // TODO : Can this become an enum
	Rv32_register_id rd;
	Rv32_register_id rs1;
	Rv_itype_imm imm;
};
//
//class Rv32i_system
//{
//public:
//
//	/*
//	Instruction execution methods
//	*/
//
//	// Integer computation: register-immediate
//
//	virtual void execute_rv32i_addi(Rv32_register_id rd, Rv32_register_id rs1, uint32_t imm) = 0;
//	//virtual void execute_rv32i_slti() = 0;
//	//virtual void execute_rv32i_andi() = 0;
//	//virtual void execute_rv32i_ori() = 0;
//	//virtual void execute_rv32i_xori() = 0;
//	//virtual void execute_rv32i_slli() = 0;
//	//virtual void execute_rv32i_srli() = 0;
//	//virtual void execute_rv32i_srai() = 0;
//
//	// 
//};

class Rv32i_decoder
{
public:
	static Rv32i_instruction_type decode_rv32i_instruction_type(uint32_t instruction);
	static Rv32i_itype_instruction decode_rv32i_itype(uint32_t instruction);
	static Rv_btype_instruction decode_btype(uint32_t instruction);
	static Rv_rtype_instruction decode_rtype(uint32_t instruction);
	static Rv_utype_instruction decode_utype(uint32_t instruction);
	static Rv32_register_id get_rv32_register_id(uint8_t encoded_register);
	static Rv32i_opcode get_rv32i_opcode(uint32_t instruction);
};

class Rv32_encoder
{
public:
	static uint32_t encode_btype(Rv32i_opcode opcode, Rv32_branch_funct3 funct3, Rv32_register_id rs1, Rv32_register_id rs2, int16_t imm);
	static uint32_t encode_op(Rv32_op_funct3 funct3, Rv32_op_funct7 funct7, Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	static uint32_t encode_op_imm(Rv32_op_imm_funct funct, Rv32_register_id rd, Rv32_register_id rs1, uint16_t imm);
	static uint32_t encode_utype(Rv32i_opcode opcode, Rv32_register_id rd, uint32_t imm);

	static uint32_t encode_add(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	static uint32_t encode_addi(Rv32_register_id rd, Rv32_register_id rs1, uint16_t imm);
	static uint32_t encode_and(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	static uint32_t encode_andi(Rv32_register_id rd, Rv32_register_id rs1, uint16_t imm);
	static uint32_t encode_auipc(Rv32_register_id rd, uint32_t imm);
	static uint32_t encode_beq(Rv32_register_id rs1, Rv32_register_id rs2, int16_t offset);
	static uint32_t encode_bge(Rv32_register_id rs1, Rv32_register_id rs2, int16_t offset);
	static uint32_t encode_bgeu(Rv32_register_id rs1, Rv32_register_id rs2, int16_t offset);
	static uint32_t encode_blt(Rv32_register_id rs1, Rv32_register_id rs2, int16_t offset);
	static uint32_t encode_bltu(Rv32_register_id rs1, Rv32_register_id rs2, int16_t offset);
	static uint32_t encode_bne(Rv32_register_id rs1, Rv32_register_id rs2, int16_t offset);
	static uint32_t encode_lui(Rv32_register_id rd, uint32_t imm);
	static uint32_t encode_or(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	static uint32_t encode_ori(Rv32_register_id rd, Rv32_register_id rs1, uint16_t imm);
	static uint32_t encode_sll(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	static uint32_t encode_slli(Rv32_register_id rd, Rv32_register_id rs1, uint8_t shift_amount);
	static uint32_t encode_slt(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	static uint32_t encode_sltu(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	static uint32_t encode_slti(Rv32_register_id rd, Rv32_register_id rs1, uint16_t imm);
	static uint32_t encode_sltiu(Rv32_register_id rd, Rv32_register_id rs1, uint16_t imm);
	static uint32_t encode_sra(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	static uint32_t encode_srai(Rv32_register_id rd, Rv32_register_id rs1, uint8_t shift_amount);
	static uint32_t encode_srl(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	static uint32_t encode_srli(Rv32_register_id rd, Rv32_register_id rs1, uint8_t shift_amount);
	static uint32_t encode_sub(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	static uint32_t encode_xori(Rv32_register_id rd, Rv32_register_id rs1, uint16_t imm);
	static uint32_t encode_xor(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
};

}
