#pragma once

#include <array>

#include "memory.h"
#include "rv32.h"

namespace riscv_sim {

class Rv32_hart
{
public:
	Rv32_hart(Memory& memory);

	void execute_add(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2);
	void execute_addi(Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm);
	void execute_and(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2);
	void execute_andi(Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm);
	void execute_auipc(Rv_register_id rd, Rv_utype_imm imm);
	void execute_beq(Rv_register_id rs1, Rv_register_id rs2, Rv_btype_imm imm);
	void execute_bge(Rv_register_id rs1, Rv_register_id rs2, Rv_btype_imm imm);
	void execute_bgeu(Rv_register_id rs1, Rv_register_id rs2, Rv_btype_imm imm);
	void execute_blt(Rv_register_id rs1, Rv_register_id rs2, Rv_btype_imm imm);
	void execute_bltu(Rv_register_id rs1, Rv_register_id rs2, Rv_btype_imm imm);
	void execute_bne(Rv_register_id rs1, Rv_register_id rs2, Rv_btype_imm imm);
	void execute_ebreak(Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm);
	void execute_ecall(Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm);
	void execute_fence(Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm);
	void execute_jal(Rv_register_id rd, Rv_jtype_imm imm);
	void execute_jalr(Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm);
	void execute_lb(Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm);
	void execute_lbu(Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm);
	void execute_lh(Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm);
	void execute_lhu(Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm);
	void execute_lw(Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm);
	void execute_lui(Rv_register_id rd, Rv_utype_imm imm);
	void execute_next();
	void execute_or(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2);
	void execute_ori(Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm);
	void execute_sb(Rv_register_id rs1, Rv_register_id rs2, Rv_stype_imm imm);
	void execute_sh(Rv_register_id rs1, Rv_register_id rs2, Rv_stype_imm imm);
	void execute_sll(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2);
	void execute_slt(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2);
	void execute_sltu(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2);
	void execute_slli(Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm);
	void execute_slti(Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm);
	void execute_sltiu(Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm);
	void execute_sra(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2);
	void execute_srai(Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm);
	void execute_srl(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2);
	void execute_srli(Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm);
	void execute_sub(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2);
	void execute_sw(Rv_register_id rs1, Rv_register_id rs2, Rv_stype_imm imm);
	void execute_xor(Rv_register_id rd, Rv_register_id rs1, Rv_register_id rs2);
	void execute_xori(Rv_register_id rd, Rv_register_id rs1, Rv_itype_imm imm);

	uint32_t get_register(Rv_register_id register_id);
	void set_register(Rv_register_id register_id, uint32_t value);

	void reset();

private:
	Memory& memory;
	std::array<uint32_t, (size_t)Rv_register_id::_count> registers;
};

}
