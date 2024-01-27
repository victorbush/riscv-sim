#pragma once

#include "rv32.h"

#include <string>

namespace riscv_sim {

struct Rv_disassembled_instruction {
	Rv32i_instruction_type type;
	Rv32_instruction_format format;
	Rv_register_id rd;
	Rv_register_id rs1;
	Rv_register_id rs2;
	int64_t imm;
};

class Rv_disassembler
{
public:

	static Rv_disassembled_instruction disassemble(uint32_t instruction);
	static const std::string& get_mnemonic(Rv32i_instruction_type type);
	static const std::string& get_register_abi_name(Rv_register_id reg);
};

}
