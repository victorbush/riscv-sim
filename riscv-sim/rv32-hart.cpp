#include <exception>
#include <map>

#include "rv32.h"
#include "rv32-hart.h"

using namespace std;

/**
The conditional branch instructions will generate an instruction-address-misaligned exception if the
target address is not aligned to a four-byte boundary and the branch condition evaluates to true.
If the branch condition evaluates to false, the instruction-address-misaligned exception will not be
raised.

Unconditional branch instructions will generate an instruction-address-misaligned exception if the
target address is not aligned to a four-byte boundary.
*/
#define throw_if_branch_target_misaligned(address) \
if (( address ) % 4 != 0) \
	throw exception("instruction-address-misaligned");

namespace riscv_sim {

Rv32_hart::Rv32_hart(Memory& memory)
	: memory(memory), registers()
{
}

// These types are function pointers to member methods that execute different types of instructions.

typedef void (Rv32_hart::* btype_executor)(Rv32_register_id rs1, Rv32_register_id rs2, Rv_btype_imm imm);
typedef void (Rv32_hart::* itype_executor)(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm);
typedef void (Rv32_hart::* jtype_executor)(Rv32_register_id rd, Rv_jtype_imm imm);
typedef void (Rv32_hart::* rtype_executor)(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
typedef void (Rv32_hart::* stype_executor)(Rv32_register_id rs1, Rv32_register_id rs2, Rv_stype_imm imm);
typedef void (Rv32_hart::* utype_executor)(Rv32_register_id rd, Rv_utype_imm imm);

/** Defines the instruction format and a pointer to the member method that executes the instruction. */
struct Instruction_executor
{
	Instruction_executor(btype_executor btype) : execute_btype(btype), format(Rv32_instruction_format::btype) {}
	Instruction_executor(itype_executor itype) : execute_itype(itype), format(Rv32_instruction_format::itype) {}
	Instruction_executor(jtype_executor jtype) : execute_jtype(jtype), format(Rv32_instruction_format::jtype) {}
	Instruction_executor(rtype_executor rtype) : execute_rtype(rtype), format(Rv32_instruction_format::rtype) {}
	Instruction_executor(stype_executor stype) : execute_stype(stype), format(Rv32_instruction_format::stype) {}
	Instruction_executor(utype_executor utype) : execute_utype(utype), format(Rv32_instruction_format::utype) {}

	Rv32_instruction_format format;

	union
	{
		btype_executor execute_btype;
		itype_executor execute_itype;
		jtype_executor execute_jtype;
		rtype_executor execute_rtype;
		stype_executor execute_stype;
		utype_executor execute_utype;
	};
};

/** Maps an instruction to the appropriate format and executor method. */
static const map<Rv32i_instruction_type, Instruction_executor> instruction_executor_map = {

	// B-type

	{ Rv32i_instruction_type::beq, &Rv32_hart::execute_beq },
	{ Rv32i_instruction_type::bge, &Rv32_hart::execute_bge },
	{ Rv32i_instruction_type::bgeu, &Rv32_hart::execute_bgeu },
	{ Rv32i_instruction_type::blt, &Rv32_hart::execute_blt },
	{ Rv32i_instruction_type::bltu, &Rv32_hart::execute_bltu },
	{ Rv32i_instruction_type::bne, &Rv32_hart::execute_bne },

	// I-type - OP-IMM

	{ Rv32i_instruction_type::addi, &Rv32_hart::execute_addi },
	{ Rv32i_instruction_type::andi, &Rv32_hart::execute_andi },
	{ Rv32i_instruction_type::ori, &Rv32_hart::execute_ori },
	{ Rv32i_instruction_type::slli, &Rv32_hart::execute_slli },
	{ Rv32i_instruction_type::slti, &Rv32_hart::execute_slti },
	{ Rv32i_instruction_type::sltiu, &Rv32_hart::execute_sltiu },
	{ Rv32i_instruction_type::srli, &Rv32_hart::execute_srli },
	{ Rv32i_instruction_type::srai, &Rv32_hart::execute_srai },
	{ Rv32i_instruction_type::xori, &Rv32_hart::execute_xori },

	// I-type - LOAD

	{ Rv32i_instruction_type::lb, &Rv32_hart::execute_lb },
	{ Rv32i_instruction_type::lbu, &Rv32_hart::execute_lbu },
	{ Rv32i_instruction_type::lh, &Rv32_hart::execute_lh },
	{ Rv32i_instruction_type::lhu, &Rv32_hart::execute_lhu },
	{ Rv32i_instruction_type::lw, &Rv32_hart::execute_lw },

	// J-type

	{ Rv32i_instruction_type::jal, &Rv32_hart::execute_jal },

	// R-type

	{ Rv32i_instruction_type::add, &Rv32_hart::execute_add },
	{ Rv32i_instruction_type::and_, &Rv32_hart::execute_and },
	{ Rv32i_instruction_type::or_, &Rv32_hart::execute_or },
	{ Rv32i_instruction_type::sub, &Rv32_hart::execute_sub },
	{ Rv32i_instruction_type::sll, &Rv32_hart::execute_sll },
	{ Rv32i_instruction_type::slt, &Rv32_hart::execute_slt },
	{ Rv32i_instruction_type::sltu, &Rv32_hart::execute_sltu },
	{ Rv32i_instruction_type::sra, &Rv32_hart::execute_sra },
	{ Rv32i_instruction_type::srl, &Rv32_hart::execute_srl },
	{ Rv32i_instruction_type::xor_, &Rv32_hart::execute_xor },

	// S-type

	{ Rv32i_instruction_type::sb, &Rv32_hart::execute_sb },
	{ Rv32i_instruction_type::sh, &Rv32_hart::execute_sh },
	{ Rv32i_instruction_type::sw, &Rv32_hart::execute_sw },

	// U-type

	{ Rv32i_instruction_type::auipc, &Rv32_hart::execute_auipc },
	{ Rv32i_instruction_type::lui, &Rv32_hart::execute_lui },
};

void Rv32_hart::execute_next()
{
	auto next_inst_addr = get_register(Rv32_register_id::pc);
	auto next_inst = memory.read_32(next_inst_addr);
	auto next_inst_type = Rv32i_decoder::decode_rv32i_instruction_type(next_inst);

	if (next_inst_type == Rv32i_instruction_type::invalid)
		throw exception("Invalid instruction.");

	if (!instruction_executor_map.contains(next_inst_type))
		throw exception("Not implemented.");

	// Branch instructions handle updating the PC register manually
	auto auto_inc_pc = true;

	auto& executor = instruction_executor_map.at(next_inst_type);
	switch (executor.format)
	{
	case Rv32_instruction_format::btype:
	{
		auto btype = Rv32i_decoder::decode_btype(next_inst);
		(*this.*(executor.execute_btype))(btype.rs1, btype.rs2, btype.imm);
		auto_inc_pc = false;
		break;
	}

	case Rv32_instruction_format::itype:
	{
		auto itype = Rv32i_decoder::decode_rv32i_itype(next_inst);
		(*this.*(executor.execute_itype))(itype.rd, itype.rs1, itype.imm);
		break;
	}
	
	case Rv32_instruction_format::jtype:
	{
		auto jtype = Rv32i_decoder::decode_jtype(next_inst);
		(*this.*(executor.execute_jtype))(jtype.rd, jtype.imm);
		auto_inc_pc = false;
		break;
	}

	case Rv32_instruction_format::rtype:
	{
		auto rtype = Rv32i_decoder::decode_rtype(next_inst);
		(*this.*(executor.execute_rtype))(rtype.rd, rtype.rs1, rtype.rs2);
		break;
	}

	case Rv32_instruction_format::stype:
	{
		auto stype = Rv32i_decoder::decode_stype(next_inst);
		(*this.*(executor.execute_stype))(stype.rs1, stype.rs2, stype.imm);
		break;
	}

	case Rv32_instruction_format::utype:
	{
		auto utype = Rv32i_decoder::decode_utype(next_inst);
		(*this.*(executor.execute_utype))(utype.rd, utype.imm);
		break;
	}

	default:
		throw exception("Not implemented.");
	}

	// Increment PC
	if (auto_inc_pc)
		set_register(Rv32_register_id::pc, get_register(Rv32_register_id::pc) + 4);
}

void Rv32_hart::execute_add(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	uint32_t rs1_val = get_register(rs1);
	uint32_t rs2_val = get_register(rs2);
	set_register(rd, rs1_val + rs2_val);
}

void Rv32_hart::execute_addi(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	int32_t source = get_register(rs1);
	int32_t immediate = imm.get_signed();
	set_register(rd, source + immediate);
}

void Rv32_hart::execute_and(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	uint32_t rs1_val = get_register(rs1);
	uint32_t rs2_val = get_register(rs2);
	set_register(rd, rs1_val & rs2_val);
}

void Rv32_hart::execute_andi(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	int32_t source = get_register(rs1);
	int32_t immediate = imm.get_signed();
	set_register(rd, source & immediate);
}

void Rv32_hart::execute_auipc(Rv32_register_id rd, Rv_utype_imm imm)
{
	uint32_t pc = get_register(Rv32_register_id::pc);
	set_register(rd, pc + imm.get_decoded());
}

void Rv32_hart::execute_beq(Rv32_register_id rs1, Rv32_register_id rs2, Rv_btype_imm imm)
{
	uint32_t pc = get_register(Rv32_register_id::pc);
	uint32_t rs1_val = get_register(rs1);
	uint32_t rs2_val = get_register(rs2);
	
	if (rs1_val == rs2_val)
	{
		pc = pc + imm.get_offset();
		throw_if_branch_target_misaligned(pc);
	}
	else
	{
		pc += 4;
	}

	set_register(Rv32_register_id::pc, pc);
}

void Rv32_hart::execute_bge(Rv32_register_id rs1, Rv32_register_id rs2, Rv_btype_imm imm)
{
	uint32_t pc = get_register(Rv32_register_id::pc);
	
	// Signed comparison
	int32_t rs1_val = get_register(rs1);
	int32_t rs2_val = get_register(rs2);

	if (rs1_val >= rs2_val)
	{
		pc = pc + imm.get_offset();
		throw_if_branch_target_misaligned(pc);
	}
	else
	{
		pc += 4;
	}

	set_register(Rv32_register_id::pc, pc);
}

void Rv32_hart::execute_bgeu(Rv32_register_id rs1, Rv32_register_id rs2, Rv_btype_imm imm)
{
	uint32_t pc = get_register(Rv32_register_id::pc);

	// Unsigned comparison
	uint32_t rs1_val = get_register(rs1);
	uint32_t rs2_val = get_register(rs2);

	if (rs1_val >= rs2_val)
	{
		pc = pc + imm.get_offset();
		throw_if_branch_target_misaligned(pc);
	}
	else
	{
		pc += 4;
	}

	set_register(Rv32_register_id::pc, pc);
}

void Rv32_hart::execute_blt(Rv32_register_id rs1, Rv32_register_id rs2, Rv_btype_imm imm)
{
	uint32_t pc = get_register(Rv32_register_id::pc);

	// Signed comparison
	int32_t rs1_val = get_register(rs1);
	int32_t rs2_val = get_register(rs2);

	if (rs1_val < rs2_val)
	{
		pc = pc + imm.get_offset();
		throw_if_branch_target_misaligned(pc);
	}
	else
	{
		pc += 4;
	}

	set_register(Rv32_register_id::pc, pc);
}

void Rv32_hart::execute_bltu(Rv32_register_id rs1, Rv32_register_id rs2, Rv_btype_imm imm)
{
	uint32_t pc = get_register(Rv32_register_id::pc);

	// Unsigned comparison
	uint32_t rs1_val = get_register(rs1);
	uint32_t rs2_val = get_register(rs2);

	if (rs1_val < rs2_val)
	{
		pc = pc + imm.get_offset();
		throw_if_branch_target_misaligned(pc);
	}
	else
	{
		pc += 4;
	}

	set_register(Rv32_register_id::pc, pc);
}

void Rv32_hart::execute_bne(Rv32_register_id rs1, Rv32_register_id rs2, Rv_btype_imm imm)
{
	uint32_t pc = get_register(Rv32_register_id::pc);
	uint32_t rs1_val = get_register(rs1);
	uint32_t rs2_val = get_register(rs2);

	if (rs1_val != rs2_val)
	{
		pc = pc + imm.get_offset();
		throw_if_branch_target_misaligned(pc);
	}
	else
	{
		pc += 4;
	}

	set_register(Rv32_register_id::pc, pc);
}

void Rv32_hart::execute_jal(Rv32_register_id rd, Rv_jtype_imm imm)
{
	const auto pc = get_register(Rv32_register_id::pc);
	uint32_t new_pc = pc + imm.get_offset();
	
	// Target must be 4-byte aligned
	throw_if_branch_target_misaligned(new_pc);

	// PC is set to the jump target (PC + Offset)
	set_register(Rv32_register_id::pc, new_pc);

	// RD is set to instruction after the jump instruction (PC + 4)
	set_register(rd, pc + 4);
}

void Rv32_hart::execute_lb(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	uint32_t rs1_val = get_register(rs1);
	int32_t offset = imm.get_signed();
	uint32_t address = rs1_val + offset;
	uint32_t mem = memory.read_8(address);

	// Sign extend
	if (mem & 0b1000'0000)
		mem |= (-1 << 8);

	set_register(rd, mem);
}

void Rv32_hart::execute_lbu(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	uint32_t rs1_val = get_register(rs1);
	int32_t offset = imm.get_signed();
	uint32_t address = rs1_val + offset;
	uint32_t mem = memory.read_8(address);

	set_register(rd, mem);
}

void Rv32_hart::execute_lh(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	uint32_t rs1_val = get_register(rs1);
	int32_t offset = imm.get_signed();
	uint32_t address = rs1_val + offset;
	uint32_t mem = memory.read_16(address);

	// Sign extend
	if (mem & (1 << 15))
		mem |= (-1 << 16);

	set_register(rd, mem);
}

void Rv32_hart::execute_lhu(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	uint32_t rs1_val = get_register(rs1);
	int32_t offset = imm.get_signed();
	uint32_t address = rs1_val + offset;
	uint32_t mem = memory.read_16(address);

	set_register(rd, mem);
}

void Rv32_hart::execute_lw(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	uint32_t rs1_val = get_register(rs1);
	int32_t offset = imm.get_signed();
	uint32_t address = rs1_val + offset;
	uint32_t mem = memory.read_32(address);

	set_register(rd, mem);
}

void Rv32_hart::execute_lui(Rv32_register_id rd, Rv_utype_imm imm)
{
	set_register(rd, imm.get_decoded());
}

void Rv32_hart::execute_or(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	uint32_t rs1_val = get_register(rs1);
	uint32_t rs2_val = get_register(rs2);
	set_register(rd, rs1_val | rs2_val);
}

void Rv32_hart::execute_ori(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	int32_t source = get_register(rs1);
	int32_t immediate = imm.get_signed();
	set_register(rd, source | immediate);
}

void Rv32_hart::execute_sb(Rv32_register_id rs1, Rv32_register_id rs2, Rv_stype_imm imm)
{
	uint32_t rs1_val = get_register(rs1);
	uint32_t rs2_val = get_register(rs2);

	int32_t offset = imm.get_offset();
	uint32_t address = rs1_val + offset;
	uint8_t val_to_write = static_cast<uint8_t>(rs2_val) & 0xFF;

	memory.write_8(address, val_to_write);
}

void Rv32_hart::execute_sh(Rv32_register_id rs1, Rv32_register_id rs2, Rv_stype_imm imm)
{
	uint32_t rs1_val = get_register(rs1);
	uint32_t rs2_val = get_register(rs2);

	int32_t offset = imm.get_offset();
	uint32_t address = rs1_val + offset;
	uint16_t val_to_write = static_cast<uint16_t>(rs2_val) & 0xFFFF;

	memory.write_16(address, val_to_write);
}

void Rv32_hart::execute_sll(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	// Logical left shift on the value in rs1 by the shift amount held in the lower 5 bits of rs2.

	uint32_t rs1_val = get_register(rs1);
	uint32_t rs2_val = get_register(rs2);
	uint32_t shift_amount = rs2_val & 0b11111;
	set_register(rd, rs1_val << shift_amount);
}

void Rv32_hart::execute_slli(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	uint32_t source = get_register(rs1);
	uint8_t shift_amount = imm.get_shift_amount();
	set_register(rd, source << shift_amount);
}

void Rv32_hart::execute_slt(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	int32_t rs1_val = get_register(rs1);
	int32_t rs2_val = get_register(rs2);
	set_register(rd, rs1_val < rs2_val);
}

void Rv32_hart::execute_slti(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	// rd = (rs1 < imm) ? 1 : 0;
	int32_t source = get_register(rs1);
	int32_t immediate = imm.get_signed();
	set_register(rd, (source < immediate) ? 1 : 0);
}

void Rv32_hart::execute_sltiu(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	// rd = (rs1 < imm) ? 1 : 0;
	uint32_t source = get_register(rs1);
	uint32_t immediate = imm.get_unsigned();
	set_register(rd, (source < immediate) ? 1 : 0);
}

void Rv32_hart::execute_sltu(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	uint32_t rs1_val = get_register(rs1);
	uint32_t rs2_val = get_register(rs2);
	set_register(rd, rs1_val < rs2_val);
}

void Rv32_hart::execute_sra(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	// Arithmetic right shift on the value in rs1 by the shift amount held in the lower 5 bits of rs2.

	int32_t rs1_val = get_register(rs1);
	int32_t rs2_val = get_register(rs2);
	int32_t shift_amount = rs2_val & 0b11111;
	set_register(rd, rs1_val >> shift_amount);
}

void Rv32_hart::execute_srai(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	// Static cast to signed integer to make it more obvious what is going on.
	// Arithemtic shift right needs signed int
	uint32_t source = get_register(rs1);
	uint8_t shift_amount = imm.get_shift_amount();
	set_register(rd, static_cast<int32_t>(source) >> shift_amount);
}

void Rv32_hart::execute_srl(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	// Logical right shift on the value in rs1 by the shift amount held in the lower 5 bits of rs2.

	uint32_t rs1_val = get_register(rs1);
	uint32_t rs2_val = get_register(rs2);
	uint32_t shift_amount = rs2_val & 0b11111;
	set_register(rd, rs1_val >> shift_amount);
}

void Rv32_hart::execute_srli(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	uint32_t source = get_register(rs1);
	uint8_t shift_amount = imm.get_shift_amount();
	set_register(rd, source >> shift_amount);
}

void Rv32_hart::execute_sub(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	uint32_t rs1_val = get_register(rs1);
	uint32_t rs2_val = get_register(rs2);
	set_register(rd, rs1_val - rs2_val);
}

void Rv32_hart::execute_sw(Rv32_register_id rs1, Rv32_register_id rs2, Rv_stype_imm imm)
{
	uint32_t rs1_val = get_register(rs1);
	uint32_t rs2_val = get_register(rs2);

	int32_t offset = imm.get_offset();
	uint32_t address = rs1_val + offset;

	memory.write_32(address, rs2_val);
}

void Rv32_hart::execute_xor(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2)
{
	uint32_t rs1_val = get_register(rs1);
	uint32_t rs2_val = get_register(rs2);
	set_register(rd, rs1_val ^ rs2_val);
}

void Rv32_hart::execute_xori(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	int32_t source = get_register(rs1);
	int32_t immediate = imm.get_signed();
	set_register(rd, source ^ immediate);
}

uint32_t Rv32_hart::get_register(Rv32_register_id register_id)
{
	return registers[static_cast<uint8_t>(register_id)];
}

void Rv32_hart::set_register(Rv32_register_id register_id, uint32_t value)
{
	// x0 is hardcoded to 0 and can't be changed. Writes to x0 are treated as a nop.
	if (register_id == Rv32_register_id::x0)
		return;

	registers[static_cast<uint8_t>(register_id)] = value;
}

}
