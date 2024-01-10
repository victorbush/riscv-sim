#include "simple-system.h"

#include <vector>
#include <utility>

using namespace std;

namespace riscv_sim {

//void Simple_system::execute_next()
//{
	//auto next_inst_addr = get_register(Rv32_register_id::pc);
	//auto next_inst = memory.read_32(next_inst_addr);
	//auto next_inst_type = Rv32i_decoder::decode_rv32i_instruction_type(next_inst);

	//switch (next_inst_type)
	//{
	//case Rv32i_instruction_type::addi:
	//{
	//	auto itype = Rv32i_decoder::decode_rv32i_itype(next_inst);
	//	execute_rv32i_addi(itype.rd, itype.rs1, itype.imm);
	//	break;
	//}

	//case Rv32i_instruction_type::invalid:
	//	throw exception("Invalid instruction.");
	//}

	//// Increment PC
	//set_register(Rv32_register_id::pc, get_register(Rv32_register_id::pc) + 4);

	//auto opcode = Rv32i_decoder::get_rv32i_opcode(next_inst);

	//const auto check_instruction = [](uint32_t actual_instruction, Rv32i_instruction_mask inst_to_check) -> bool {
	//	return (actual_instruction & inst_to_check.mask) == inst_to_check.value;
	//};

	//if (check_instruction(next_inst, Rv32i_instruction_mask::add))
	//	execute_rv32i_addi()

	//switch (opcode)
	//{
	//case Rv32i_opcode::op:
	//{
	//	auto funct3 = Rv32i_decoder::
	//}
	//	break;
	//}
//}

void Simple_system::load_program(std::span<uint32_t> instructions)
{
	auto next_addr = 0;

	for (auto i : instructions) {
		memory.write_32(next_addr, i);
		next_addr += 4;
	}

	hart.set_register(Rv32_register_id::pc, 0);
}

/* ========================================================
Simple memory subsystem
======================================================== */

void Simple_memory_subsystem::write_byte(uint32_t address, uint8_t value)
{
	memory[address] = value;
}

void Simple_memory_subsystem::write_32(uint32_t address, uint32_t value)
{
	memory[address] = 0xFF & value;
	memory[address + 1] = 0xFF & (value >> 8);
	memory[address + 2] = 0xFF & (value >> 16);
	memory[address + 3] = 0xFF & (value >> 24);
}

uint8_t Simple_memory_subsystem::read_byte(uint32_t address)
{
	return memory[address];
}

uint32_t Simple_memory_subsystem::read_32(uint32_t address)
{
	return (memory[address]
		| (memory[address + 1] << 8)
		| (memory[address + 2] << 16)
		| (memory[address + 3] << 24));
}

}
