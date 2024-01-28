#include "rv-disassembler.h"

#include <map>
#include <string>

using namespace std;

namespace riscv_sim {

typedef Rv_disassembled_instruction(*disassembly_func)(uint32_t instruction, Rv32i_instruction_type type);

static constexpr string c_unknown = "unknown";

static Rv_disassembled_instruction disassemble_btype(uint32_t instruction, Rv32i_instruction_type type)
{
	auto btype = Rv32_decoder::decode_btype(instruction);

	auto dis = Rv_disassembled_instruction();
	dis.type = type;
	dis.format = Rv32_instruction_format::btype;
	dis.rd = Rv_register_id::_unused;
	dis.rs1 = btype.rs1;
	dis.rs2 = btype.rs2;
	dis.imm = btype.imm.get_offset();
	return dis;
}

static Rv_disassembled_instruction disassemble_itype(uint32_t instruction, Rv32i_instruction_type type)
{
	auto itype = Rv32_decoder::decode_itype(instruction);

	auto dis = Rv_disassembled_instruction();
	dis.type = type;
	dis.format = Rv32_instruction_format::itype;
	dis.rd = itype.rd;
	dis.rs1 = itype.rs1;
	dis.rs2 = Rv_register_id::_unused;
	dis.imm = itype.imm.get_signed();
	return dis;
}

static Rv_disassembled_instruction disassemble_jtype(uint32_t instruction, Rv32i_instruction_type type)
{
	auto jtype = Rv32_decoder::decode_jtype(instruction);

	auto dis = Rv_disassembled_instruction();
	dis.type = type;
	dis.format = Rv32_instruction_format::jtype;
	dis.rd = jtype.rd;
	dis.rs1 = Rv_register_id::_unused;
	dis.rs2 = Rv_register_id::_unused;
	dis.imm = jtype.imm.get_offset();
	return dis;
}

static Rv_disassembled_instruction disassemble_rtype(uint32_t instruction, Rv32i_instruction_type type)
{
	auto rtype = Rv32_decoder::decode_rtype(instruction);

	auto dis = Rv_disassembled_instruction();
	dis.type = type;
	dis.format = Rv32_instruction_format::rtype;
	dis.rd = rtype.rd;
	dis.rs1 = rtype.rs1;
	dis.rs2 = rtype.rs2;
	dis.imm = 0;
	return dis;
}

static Rv_disassembled_instruction disassemble_stype(uint32_t instruction, Rv32i_instruction_type type)
{
	auto stype = Rv32_decoder::decode_stype(instruction);

	auto dis = Rv_disassembled_instruction();
	dis.type = type;
	dis.format = Rv32_instruction_format::stype;
	dis.rd = Rv_register_id::_unused;
	dis.rs1 = stype.rs1;
	dis.rs2 = stype.rs2;
	dis.imm = stype.imm.get_offset();
	return dis;
}

static Rv_disassembled_instruction disassemble_utype(uint32_t instruction, Rv32i_instruction_type type)
{
	auto utype = Rv32_decoder::decode_utype(instruction);

	auto dis = Rv_disassembled_instruction();
	dis.type = type;
	dis.format = Rv32_instruction_format::utype;
	dis.rd = utype.rd;
	dis.rs1 = Rv_register_id::_unused;
	dis.rs2 = Rv_register_id::_unused;
	dis.imm = utype.imm.get_decoded();
	return dis;
}

static const map<Rv32i_instruction_type, disassembly_func> s_disassembly_func_map = {

	// B-type

	{ Rv32i_instruction_type::beq, &disassemble_btype },
	{ Rv32i_instruction_type::bge, &disassemble_btype },
	{ Rv32i_instruction_type::bgeu, &disassemble_btype },
	{ Rv32i_instruction_type::blt, &disassemble_btype },
	{ Rv32i_instruction_type::bltu, &disassemble_btype },
	{ Rv32i_instruction_type::bne, &disassemble_btype },

	// I-type - JALR

	{ Rv32i_instruction_type::jalr, &disassemble_itype },

	// I-type - LOAD

	{ Rv32i_instruction_type::lb, &disassemble_itype },
	{ Rv32i_instruction_type::lbu, &disassemble_itype },
	{ Rv32i_instruction_type::lh, &disassemble_itype },
	{ Rv32i_instruction_type::lhu, &disassemble_itype },
	{ Rv32i_instruction_type::lw, &disassemble_itype },

	// I-type - MISC-MEM

	{ Rv32i_instruction_type::fence, &disassemble_itype },

	// I-type - OP-IMM

	{ Rv32i_instruction_type::addi, &disassemble_itype },
	{ Rv32i_instruction_type::andi, &disassemble_itype },
	{ Rv32i_instruction_type::ori, &disassemble_itype },
	{ Rv32i_instruction_type::slli, &disassemble_itype },
	{ Rv32i_instruction_type::slti, &disassemble_itype },
	{ Rv32i_instruction_type::sltiu, &disassemble_itype },
	{ Rv32i_instruction_type::srli, &disassemble_itype },
	{ Rv32i_instruction_type::srai, &disassemble_itype },
	{ Rv32i_instruction_type::xori, &disassemble_itype },

	// I-type - SYSTEM

	{ Rv32i_instruction_type::ebreak, &disassemble_itype },
	{ Rv32i_instruction_type::ecall, &disassemble_itype },

	// J-type

	{ Rv32i_instruction_type::jal, &disassemble_jtype },

	// R-type

	{ Rv32i_instruction_type::add, &disassemble_rtype },
	{ Rv32i_instruction_type::and_, &disassemble_rtype },
	{ Rv32i_instruction_type::or_, &disassemble_rtype },
	{ Rv32i_instruction_type::sub, &disassemble_rtype },
	{ Rv32i_instruction_type::sll, &disassemble_rtype },
	{ Rv32i_instruction_type::slt, &disassemble_rtype },
	{ Rv32i_instruction_type::sltu, &disassemble_rtype },
	{ Rv32i_instruction_type::sra, &disassemble_rtype },
	{ Rv32i_instruction_type::srl, &disassemble_rtype },
	{ Rv32i_instruction_type::xor_, &disassemble_rtype },

	// S-type

	{ Rv32i_instruction_type::sb, &disassemble_stype },
	{ Rv32i_instruction_type::sh, &disassemble_stype },
	{ Rv32i_instruction_type::sw, &disassemble_stype },

	// U-type

	{ Rv32i_instruction_type::auipc, &disassemble_utype },
	{ Rv32i_instruction_type::lui, &disassemble_utype },
};

static const map<Rv32i_instruction_type, string> s_mnemonic_map = {

	// B-type

	{ Rv32i_instruction_type::beq, "beq" },
	{ Rv32i_instruction_type::bge, "bge"},
	{ Rv32i_instruction_type::bgeu, "bgeu"},
	{ Rv32i_instruction_type::blt, "blt" },
	{ Rv32i_instruction_type::bltu, "bltu" },
	{ Rv32i_instruction_type::bne, "bne" },

	// I-type - JALR

	{ Rv32i_instruction_type::jalr, "jalr" },

	// I-type - LOAD

	{ Rv32i_instruction_type::lb, "lb" },
	{ Rv32i_instruction_type::lbu, "lbu" },
	{ Rv32i_instruction_type::lh, "lh" },
	{ Rv32i_instruction_type::lhu, "lhu" },
	{ Rv32i_instruction_type::lw, "lw" },

	// I-type - MISC-MEM

	{ Rv32i_instruction_type::fence, "fence" },

	// I-type - OP-IMM

	{ Rv32i_instruction_type::addi, "addi" },
	{ Rv32i_instruction_type::andi, "andi" },
	{ Rv32i_instruction_type::ori, "ori" },
	{ Rv32i_instruction_type::slli, "slli" },
	{ Rv32i_instruction_type::slti, "slti" },
	{ Rv32i_instruction_type::sltiu, "sltiu" },
	{ Rv32i_instruction_type::srli, "srli"},
	{ Rv32i_instruction_type::srai, "srai" },
	{ Rv32i_instruction_type::xori, "xori" },

	// I-type - SYSTEM

	{ Rv32i_instruction_type::ebreak, "ebreak" },
	{ Rv32i_instruction_type::ecall, "ecall" },

	// J-type

	{ Rv32i_instruction_type::jal, "jal" },

	// R-type

	{ Rv32i_instruction_type::add, "add" },
	{ Rv32i_instruction_type::and_, "and" },
	{ Rv32i_instruction_type::or_, "or" },
	{ Rv32i_instruction_type::sub, "sub" },
	{ Rv32i_instruction_type::sll, "sll" },
	{ Rv32i_instruction_type::slt, "slt" },
	{ Rv32i_instruction_type::sltu, "sltu" },
	{ Rv32i_instruction_type::sra, "sra" },
	{ Rv32i_instruction_type::srl, "srl" },
	{ Rv32i_instruction_type::xor_, "xor" },

	// S-type

	{ Rv32i_instruction_type::sb, "sb" },
	{ Rv32i_instruction_type::sh, "sh" },
	{ Rv32i_instruction_type::sw, "sw" },

	// U-type

	{ Rv32i_instruction_type::auipc, "auipc" },
	{ Rv32i_instruction_type::lui, "lui" },
};

static const map<Rv_register_id, string> s_register_abi_name_map = {
	{ Rv_register_id::zero, "zero" },
	{ Rv_register_id::ra, "ra" },
	{ Rv_register_id::sp, "sp" },
	{ Rv_register_id::gp, "gp" },
	{ Rv_register_id::tp, "tp" },
	{ Rv_register_id::t0, "t0" },
	{ Rv_register_id::t1, "t1" },
	{ Rv_register_id::t2, "t2" },
	{ Rv_register_id::s0, "s0" },
	{ Rv_register_id::fp, "fp" },
	{ Rv_register_id::s1, "s1" },
	{ Rv_register_id::a0, "a0" },
	{ Rv_register_id::a1, "a1" },
	{ Rv_register_id::a2, "a2" },
	{ Rv_register_id::a3, "a3" },
	{ Rv_register_id::a4, "a4" },
	{ Rv_register_id::a5, "a5" },
	{ Rv_register_id::a6, "a6" },
	{ Rv_register_id::a7, "a7" },
	{ Rv_register_id::s2, "s2" },
	{ Rv_register_id::s3, "s3" },
	{ Rv_register_id::s4, "s4" },
	{ Rv_register_id::s5, "s5" },
	{ Rv_register_id::s6, "s6" },
	{ Rv_register_id::s7, "s7" },
	{ Rv_register_id::s8, "s8" },
	{ Rv_register_id::s9, "s9" },
	{ Rv_register_id::s10, "s10" },
	{ Rv_register_id::s11, "s11" },
	{ Rv_register_id::t3, "t3" },
	{ Rv_register_id::t4, "t4" },
	{ Rv_register_id::t5, "t5" },
	{ Rv_register_id::t6, "t6" },
	{ Rv_register_id::pc, "pc" },
};

Rv_disassembled_instruction Rv_disassembler::disassemble(uint32_t instruction)
{
	auto type = Rv32_decoder::decode_instruction_type(instruction);
	if (!s_disassembly_func_map.contains(type))
	{
		auto dis = Rv_disassembled_instruction();
		dis.type = type;
		dis.format = Rv32_instruction_format::itype;
		dis.rd = Rv_register_id::_unused;
		dis.rs1 = Rv_register_id::_unused;
		dis.rs2 = Rv_register_id::_unused;
		dis.imm = 0;
		return dis;
	}

	auto disassembler = s_disassembly_func_map.at(type);
	return disassembler(instruction, type);
}

const string& Rv_disassembler::get_mnemonic(Rv32i_instruction_type type)
{
	if (s_mnemonic_map.contains(type))
		return s_mnemonic_map.at(type);

	return c_unknown;
}

const string& Rv_disassembler::get_register_abi_name(Rv_register_id reg)
{
	if (s_register_abi_name_map.contains(reg))
		return s_register_abi_name_map.at(reg);

	return c_unknown;
}

}
