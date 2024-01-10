#include <exception>
#include <map>

#include "rv32.h"
#include "rv32-hart.h"

using namespace std;

namespace riscv_sim {

Rv32_hart::Rv32_hart(Memory& memory)
	: memory(memory), registers()
{
}

typedef void (Rv32_hart::* itype_executor)(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm);
typedef void (Rv32_hart::* rtype_executor)(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
typedef void (Rv32_hart::* utype_executor)(Rv32_register_id rd, Rv_utype_imm imm);

//union executor_lookup
//{
//	executor_lookup(itype_executor itype) : itype_executor(itype) {}
//	executor_lookup(rtype_executor rtype) : rtype_executor(rtype) {}
//
//	itype_executor itype_executor;
//	rtype_executor rtype_executor;
//};

struct instruction_executor
{
	instruction_executor(itype_executor itype) : execute_itype(itype), type(Rv32_instruction_type::itype) {}
	instruction_executor(rtype_executor rtype) : execute_rtype(rtype), type(Rv32_instruction_type::rtype) {}
	instruction_executor(utype_executor utype) : execute_utype(utype), type(Rv32_instruction_type::utype) {}

	Rv32_instruction_type type;

	union
	{
		itype_executor execute_itype;
		rtype_executor execute_rtype;
		utype_executor execute_utype;
	};
};

//static const map<Rv32i_instruction_type, itype_executor> itype_executor_map = {
//
//	{ Rv32i_instruction_type::addi, Rv32_hart::execute_addi },
//	{ Rv32i_instruction_type::andi, Rv32_hart::execute_andi },
//	{ Rv32i_instruction_type::ori, Rv32_hart::execute_ori },
//	{ Rv32i_instruction_type::slli, Rv32_hart::execute_slli },
//	{ Rv32i_instruction_type::slti, Rv32_hart::execute_slti },
//	{ Rv32i_instruction_type::sltiu, Rv32_hart::execute_sltiu },
//	{ Rv32i_instruction_type::srli, Rv32_hart::execute_srli },
//	{ Rv32i_instruction_type::srai, Rv32_hart::execute_srai },
//	{ Rv32i_instruction_type::xori, Rv32_hart::execute_xori },
//};
//
//static const map<Rv32i_instruction_type, rtype_executor> rtype_executor_map = {
//
//	{ Rv32i_instruction_type::add, Rv32_hart::execute_add },
//	{ Rv32i_instruction_type::and_, Rv32_hart::execute_and },
//	{ Rv32i_instruction_type::or_, Rv32_hart::execute_or },
//	{ Rv32i_instruction_type::sub, Rv32_hart::execute_sub },
//	{ Rv32i_instruction_type::sll, Rv32_hart::execute_sll },
//	{ Rv32i_instruction_type::slt, Rv32_hart::execute_slt },
//	{ Rv32i_instruction_type::sltu, Rv32_hart::execute_sltu },
//	{ Rv32i_instruction_type::sra, Rv32_hart::execute_sra },
//	{ Rv32i_instruction_type::srl, Rv32_hart::execute_srl },
//	{ Rv32i_instruction_type::xor_, Rv32_hart::execute_xor },
//};
//
//static const map<Rv32i_instruction_type, Rv32_instruction_type> instruction_type_map = {
//
//	{ Rv32i_instruction_type::add, Rv32_instruction_type::rtype },
//
//	{ Rv32i_instruction_type::addi, Rv32_instruction_type::itype },
//};

static const map<Rv32i_instruction_type, instruction_executor> instruction_executor_map = {

	// I-type

	{ Rv32i_instruction_type::addi, &Rv32_hart::execute_addi },
	{ Rv32i_instruction_type::andi, &Rv32_hart::execute_andi },
	{ Rv32i_instruction_type::ori, &Rv32_hart::execute_ori },
	{ Rv32i_instruction_type::slli, &Rv32_hart::execute_slli },
	{ Rv32i_instruction_type::slti, &Rv32_hart::execute_slti },
	{ Rv32i_instruction_type::sltiu, &Rv32_hart::execute_sltiu },
	{ Rv32i_instruction_type::srli, &Rv32_hart::execute_srli },
	{ Rv32i_instruction_type::srai, &Rv32_hart::execute_srai },
	{ Rv32i_instruction_type::xori, &Rv32_hart::execute_xori },

	// R-type

	{ Rv32i_instruction_type::add, &Rv32_hart::execute_add },
	//{ Rv32i_instruction_type::and_, &Rv32_hart::execute_and },
	//{ Rv32i_instruction_type::or_, &Rv32_hart::execute_or },
	//{ Rv32i_instruction_type::sub, &Rv32_hart::execute_sub },
	//{ Rv32i_instruction_type::sll, &Rv32_hart::execute_sll },
	//{ Rv32i_instruction_type::slt, &Rv32_hart::execute_slt },
	//{ Rv32i_instruction_type::sltu, &Rv32_hart::execute_sltu },
	//{ Rv32i_instruction_type::sra, &Rv32_hart::execute_sra },
	//{ Rv32i_instruction_type::srl, &Rv32_hart::execute_srl },
	//{ Rv32i_instruction_type::xor_, &Rv32_hart::execute_xor },

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

	auto executor = instruction_executor_map.at(next_inst_type);
	switch (executor.type)
	{
	case Rv32_instruction_type::btype:
	{
		throw exception("Not implemented.");
	}

	case Rv32_instruction_type::itype:
	{
		auto itype = Rv32i_decoder::decode_rv32i_itype(next_inst);
		(*this.*(executor.execute_itype))(itype.rd, itype.rs1, itype.imm);
		break;
	}
	
	case Rv32_instruction_type::jtype:
	{
		throw exception("Not implemented.");
	}

	case Rv32_instruction_type::rtype:
	{
		auto rtype = Rv32i_decoder::decode_rtype(next_inst);
		(*this.*(executor.execute_rtype))(rtype.rd, rtype.rs1, rtype.rs2);
		break;
	}

	case Rv32_instruction_type::stype:
	{
		throw exception("Not implemented.");
	}

	case Rv32_instruction_type::utype:
	{
		auto utype = Rv32i_decoder::decode_utype(next_inst);
		(*this.*(executor.execute_utype))(utype.rd, utype.imm);
		break;
	}

	default:
		throw exception("Not implemented.");
	}

	// Increment PC
	set_register(Rv32_register_id::pc, get_register(Rv32_register_id::pc) + 4);

	//switch (next_inst_type)
	//{

	//// u-type

	//case Rv32i_instruction_type::auipc:
	//{
	//	auto utype = Rv32i_decoder::decode_utype(next_inst);
	//	execute_auipc(utype.rd, utype.imm);
	//	break;
	//}

	//case Rv32i_instruction_type::lui:
	//{
	//	auto utype = Rv32i_decoder::decode_utype(next_inst);
	//	execute_lui(utype.rd, utype.imm);
	//	break;
	//}

	//// i-type

	//case Rv32i_instruction_type::addi:
	//{
	//	auto itype = Rv32i_decoder::decode_rv32i_itype(next_inst);
	//	execute_addi(itype.rd, itype.rs1, itype.imm);
	//	break;
	//}

	//case Rv32i_instruction_type::andi:
	//{
	//	auto itype = Rv32i_decoder::decode_rv32i_itype(next_inst);
	//	execute_andi(itype.rd, itype.rs1, itype.imm);
	//	break;
	//}

	//case Rv32i_instruction_type::ori:
	//{
	//	auto itype = Rv32i_decoder::decode_rv32i_itype(next_inst);
	//	execute_ori(itype.rd, itype.rs1, itype.imm);
	//	break;
	//}

	//case Rv32i_instruction_type::slli:
	//{
	//	auto itype = Rv32i_decoder::decode_rv32i_itype(next_inst);
	//	execute_slli(itype.rd, itype.rs1, itype.imm);
	//	break;
	//}

	//case Rv32i_instruction_type::slti:
	//{
	//	auto itype = Rv32i_decoder::decode_rv32i_itype(next_inst);
	//	execute_slti(itype.rd, itype.rs1, itype.imm);
	//	break;
	//}

	//case Rv32i_instruction_type::sltiu:
	//{
	//	auto itype = Rv32i_decoder::decode_rv32i_itype(next_inst);
	//	execute_sltiu(itype.rd, itype.rs1, itype.imm);
	//	break;
	//}

	//case Rv32i_instruction_type::srai:
	//{
	//	auto itype = Rv32i_decoder::decode_rv32i_itype(next_inst);
	//	execute_srai(itype.rd, itype.rs1, itype.imm);
	//	break;
	//}

	//case Rv32i_instruction_type::srli:
	//{
	//	auto itype = Rv32i_decoder::decode_rv32i_itype(next_inst);
	//	execute_srli(itype.rd, itype.rs1, itype.imm);
	//	break;
	//}

	//case Rv32i_instruction_type::xori:
	//{
	//	auto itype = Rv32i_decoder::decode_rv32i_itype(next_inst);
	//	execute_xori(itype.rd, itype.rs1, itype.imm);
	//	break;
	//}

	//// r-type

	//case Rv32i_instruction_type::add:
	//{
	//	auto rtype = Rv32i_decoder::decode_rtype(next_inst);
	//	execute_add(rtype.rd, rtype.rs1, rtype.rs2);
	//	break;
	//}

	//// invalid

	//case Rv32i_instruction_type::invalid:
	//default:
	//	throw exception("Invalid instruction.");
	//}

	//// Increment PC
	//set_register(Rv32_register_id::pc, get_register(Rv32_register_id::pc) + 4);
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
	int32_t immediate = imm.to_i32();
	set_register(rd, source + immediate);
}

void Rv32_hart::execute_andi(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	int32_t source = get_register(rs1);
	int32_t immediate = imm.to_i32();
	set_register(rd, source & immediate);
}

void Rv32_hart::execute_auipc(Rv32_register_id rd, Rv_utype_imm imm)
{
	uint32_t pc = get_register(Rv32_register_id::pc);
	set_register(rd, pc + imm.get_decoded());
}

void Rv32_hart::execute_lui(Rv32_register_id rd, Rv_utype_imm imm)
{
	set_register(rd, imm.get_decoded());
}

void Rv32_hart::execute_ori(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	int32_t source = get_register(rs1);
	int32_t immediate = imm.to_i32();
	set_register(rd, source | immediate);
}

void Rv32_hart::execute_slli(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	uint32_t source = get_register(rs1);
	uint8_t shift_amount = imm.get_shift_amount();
	set_register(rd, source << shift_amount);
}

void Rv32_hart::execute_slti(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	// rd = (rs1 < imm) ? 1 : 0;
	int32_t source = get_register(rs1);
	int32_t immediate = imm.to_i32();
	set_register(rd, (source < immediate) ? 1 : 0);
}

void Rv32_hart::execute_sltiu(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	// rd = (rs1 < imm) ? 1 : 0;
	uint32_t source = get_register(rs1);
	uint32_t immediate = imm.to_u32();
	set_register(rd, (source < immediate) ? 1 : 0);
}

void Rv32_hart::execute_srai(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	// Static cast to signed integer to make it more obvious what is going on.
	// Arithemtic shift right needs signed int
	uint32_t source = get_register(rs1);
	uint8_t shift_amount = imm.get_shift_amount();
	set_register(rd, static_cast<int32_t>(source) >> shift_amount);
}

void Rv32_hart::execute_srli(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	uint32_t source = get_register(rs1);
	uint8_t shift_amount = imm.get_shift_amount();
	set_register(rd, source >> shift_amount);
}

void Rv32_hart::execute_xori(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm)
{
	int32_t source = get_register(rs1);
	int32_t immediate = imm.to_i32();
	set_register(rd, source ^ immediate);
}

inline uint32_t Rv32_hart::get_register(Rv32_register_id register_id)
{
	return registers[static_cast<uint8_t>(register_id)];
}

inline void Rv32_hart::set_register(Rv32_register_id register_id, uint32_t value)
{
	// x0 is hardcoded to 0 and can't be changed. Writes to x0 are treated as a nop.
	if (register_id == Rv32_register_id::x0)
		return;

	registers[static_cast<uint8_t>(register_id)] = value;
}

}
