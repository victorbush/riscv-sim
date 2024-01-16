#pragma once

#include <array>

#include "memory.h"
#include "rv32.h"

namespace riscv_sim {

class Rv32_hart
{
public:
	Rv32_hart(Memory& memory);

	void execute_add(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	void execute_addi(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm);
	void execute_and(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	void execute_andi(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm);
	void execute_auipc(Rv32_register_id rd, Rv_utype_imm imm);
	void execute_beq(Rv32_register_id rs1, Rv32_register_id rs2, Rv_btype_imm imm);
	void execute_bge(Rv32_register_id rs1, Rv32_register_id rs2, Rv_btype_imm imm);
	void execute_bgeu(Rv32_register_id rs1, Rv32_register_id rs2, Rv_btype_imm imm);
	void execute_blt(Rv32_register_id rs1, Rv32_register_id rs2, Rv_btype_imm imm);
	void execute_bltu(Rv32_register_id rs1, Rv32_register_id rs2, Rv_btype_imm imm);
	void execute_bne(Rv32_register_id rs1, Rv32_register_id rs2, Rv_btype_imm imm);
	void execute_lb(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm);
	void execute_lbu(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm);
	void execute_lh(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm);
	void execute_lhu(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm);
	void execute_lw(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm);
	void execute_lui(Rv32_register_id rd, Rv_utype_imm imm);
	void execute_next();
	void execute_or(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	void execute_ori(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm);
	void execute_sb(Rv32_register_id rs1, Rv32_register_id rs2, Rv_stype_imm imm);
	void execute_sh(Rv32_register_id rs1, Rv32_register_id rs2, Rv_stype_imm imm);
	void execute_sll(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	void execute_slt(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	void execute_sltu(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	void execute_slli(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm);
	void execute_slti(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm);
	void execute_sltiu(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm);
	void execute_sra(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	void execute_srai(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm);
	void execute_srl(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	void execute_srli(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm);
	void execute_sub(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	void execute_sw(Rv32_register_id rs1, Rv32_register_id rs2, Rv_stype_imm imm);
	void execute_xor(Rv32_register_id rd, Rv32_register_id rs1, Rv32_register_id rs2);
	void execute_xori(Rv32_register_id rd, Rv32_register_id rs1, Rv_itype_imm imm);

	uint32_t get_register(Rv32_register_id register_id);
	void set_register(Rv32_register_id register_id, uint32_t value);

private:
	Memory& memory;
	std::array<uint32_t, (size_t)Rv32_register_id::_count> registers;
};

}
