#pragma once

#include <cstdint>

namespace riscv_sim {

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

/** 12-bit immediate value used by I-type instructions. The value is sign extended to 32 bits. The 12th bit determines the sign. */
struct Rv_itype_imm
{
	Rv_itype_imm();
	Rv_itype_imm(uint16_t val);

	int32_t to_i32() const;
	uint32_t to_u32() const;

	/** For shift immediate instructions, gets the shift amount. */
	uint8_t get_shift_amount() const;

private:
	uint32_t value;
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

enum class Rv32_op_funct3 : uint8_t
{
	add = 0b000,
	sub = add,
	sll = 0b001,
	slt = 0b010,
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

//
//enum class Rv32i_opcode_funct : uint16_t
//{
//	// op_imm functions
//
//	addi = 0b000,
//	slti = 0b010,
//	sltiu = 0b011,
//	xori = 0b100,
//	ori = 0b110,
//	andi = 0b111,
//
//	// op_imm - r type
//
//	slli = 0b001,
//	srli = 0b101,
//	srai = 0b0100000101,
//
//	// op - r type
//
//	add = 0b0000000000,
//	sub = 0b0100000000,
//	sll = 0b0000000001,
//	slt = 0b0000000010,
//	sltu = 0b0000000011,
//	xor_ = 0b0000000100,
//	srl = 0b0000000101,
//	sra = 0b0100000101,
//	or_ = 0b0000000110,
//	and_ = 0b0000000111,
//};

enum class Rv32i_instruction_type
{
	invalid,

	load,
	store,
	miscmem,
	system,

	lui,     // Load upper immediate
	auipc,   // Add upper immediate to PR register
	jal,     // Jump and link
	jalr,    // Jump and link register (indirect jump)

	// Conditional branches

	beq,  // Branch if rs1 == rs2
	bne,  // Branch if rs1 != rs2
	blt,  // Branch if rs1 < rs2 (signed)
	bltu, // Branch if rs1 < rs2 (unsigned)
	bge,  // Branch if rs1 >= rs2 (signed)
	bgeu, // Branch if rs1 >= rs2 (unsigned)

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

	add,  // Add
	sub,  // Subtract
	sll,
	slt,
	sltu,
	xor_,
	srl,
	sra,
	or_,
	and_,
};

//struct Rv32i_instruction_mask
//{
//	/*
//	Static defined instruction masks
//	*/

//	static Rv32i_instruction_mask lui;
//	static Rv32i_instruction_mask auipc;
//	static Rv32i_instruction_mask jal;
//	static Rv32i_instruction_mask jalr;

//	static Rv32i_instruction_mask beq;
//	static Rv32i_instruction_mask bne;
//	static Rv32i_instruction_mask blt;
//	static Rv32i_instruction_mask bge;
//	static Rv32i_instruction_mask bltu;
//	static Rv32i_instruction_mask bgeu;

//	static Rv32i_instruction_mask lb;
//	static Rv32i_instruction_mask lh;
//	static Rv32i_instruction_mask lw;
//	static Rv32i_instruction_mask lbu;
//	static Rv32i_instruction_mask lhu;

//	static Rv32i_instruction_mask sb;
//	static Rv32i_instruction_mask sh;
//	static Rv32i_instruction_mask sw;

//	static Rv32i_instruction_mask addi;
//	static Rv32i_instruction_mask slti;
//	static Rv32i_instruction_mask sltiu;
//	static Rv32i_instruction_mask xori;
//	static Rv32i_instruction_mask ori;
//	static Rv32i_instruction_mask andi;

//	static Rv32i_instruction_mask slli;
//	static Rv32i_instruction_mask srli;
//	static Rv32i_instruction_mask srai;

//	static Rv32i_instruction_mask add;
//	static Rv32i_instruction_mask sub;
//	static Rv32i_instruction_mask sll;
//	static Rv32i_instruction_mask slt;
//	static Rv32i_instruction_mask sltu;
//	static Rv32i_instruction_mask xor_;
//	static Rv32i_instruction_mask srl;
//	static Rv32i_instruction_mask sra;
//	static Rv32i_instruction_mask or_;
//	static Rv32i_instruction_mask and_;

//	static Rv32i_instruction_mask fence;
//	static Rv32i_instruction_mask ecall;
//	static Rv32i_instruction_mask ebreak;

//	/*
//	Creation helpers
//	*/

//	static Rv32i_instruction_mask create_rtype(uint8_t opcode, uint8_t funct3, uint8_t funct7)
//	{
//		return Rv32i_instruction_mask(0b1111111'00000'00000'111'00000'1111111, opcode | (funct3 << 12) | (funct7 << 25));
//	}

//	static Rv32i_instruction_mask create_itype(uint8_t opcode, uint8_t funct3)
//	{
//		return Rv32i_instruction_mask(0b111111111111'00000'111'00000'1111111, opcode | (funct3 << 12));
//	}

//	static Rv32i_instruction_mask create_stype(uint8_t opcode, uint8_t funct3)
//	{
//		return create_itype(opcode, funct3);
//	}

//	static Rv32i_instruction_mask create_btype(uint8_t opcode, uint8_t funct3)
//	{
//		return create_itype(opcode, funct3);
//	}

//	static Rv32i_instruction_mask create_utype(uint8_t opcode)
//	{
//		return Rv32i_instruction_mask(0b00000000000000000000'00000'1111111, opcode);
//	}

//	static Rv32i_instruction_mask create_jtype(uint8_t opcode)
//	{
//		return create_utype(opcode);
//	}

//	/*
//	Struct members
//	*/

//	Rv32i_instruction_mask(uint32_t mask, uint32_t value) : mask(mask), value(value) {}

//	uint32_t mask;  // The bits we use to identify a specific type of instruction
//	uint32_t value; // The expected value of this instruction after the mask is applied
//};

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
	static Rv_rtype_instruction decode_rtype(uint32_t instruction);
	static Rv_utype_instruction decode_utype(uint32_t instruction);
	static Rv32_register_id get_rv32_register_id(uint8_t encoded_register);
	static Rv32i_opcode get_rv32i_opcode(uint32_t instruction);
};

class Rv32_encoder
{
public:
	static uint32_t encode_op(Rv32_op_funct3 funct3, Rv32_op_funct7 funct7, Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	static uint32_t encode_op_imm(Rv32_op_imm_funct funct, Rv32_register_id rd, Rv32_register_id rs1, uint16_t imm);
	static uint32_t encode_utype(Rv32i_opcode opcode, Rv32_register_id rd, uint32_t imm);

	static uint32_t encode_add(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	static uint32_t encode_addi(Rv32_register_id rd, Rv32_register_id rs1, uint16_t imm);
	static uint32_t encode_and(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	static uint32_t encode_andi(Rv32_register_id rd, Rv32_register_id rs1, uint16_t imm);
	static uint32_t encode_auipc(Rv32_register_id rd, uint32_t imm);
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
