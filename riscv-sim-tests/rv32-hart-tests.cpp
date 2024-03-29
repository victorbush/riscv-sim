#include <gtest/gtest.h>
#include <map>

#include "rv32.h"
#include "rv32-hart.h"
#include "simple-system.h"
#include "test-utils.h"

using namespace riscv_sim;

/* --------------------------------------------------------
execute_next
-------------------------------------------------------- */

TEST(execute_next, ADD) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_add(Rv_register_id::x2, Rv_register_id::x3, Rv_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 4);
	hart.set_register(Rv_register_id::x4, 10);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 14);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x4), 10);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, ADDI) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_addi(Rv_register_id::x2, Rv_register_id::x3, 5);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 9);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, AND) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_and(Rv_register_id::x2, Rv_register_id::x3, Rv_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 0b1010);
	hart.set_register(Rv_register_id::x4, 0b1001);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0b1000);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0b1010);
	EXPECT_EQ(hart.get_register(Rv_register_id::x4), 0b1001);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, ANDI) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_andi(Rv_register_id::x2, Rv_register_id::x3, 0b010);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 0b011);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0b010);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0b011);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, AUIPC) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_auipc(Rv_register_id::x2, 0b1111);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), (0x500 + 0b1111'0000'0000'0000));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, BEQ) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_beq(Rv_register_id::x2, Rv_register_id::x3, 0x10);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x2, 1);
	hart.set_register(Rv_register_id::x3, 1);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 1);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x510);
}

TEST(execute_next, BGE) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_bge(Rv_register_id::x2, Rv_register_id::x3, 0x10);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x2, 3);
	hart.set_register(Rv_register_id::x3, 2);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 2);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x510);
}

TEST(execute_next, BGEU) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_bgeu(Rv_register_id::x2, Rv_register_id::x3, 0x10);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x2, -2);
	hart.set_register(Rv_register_id::x3, 3);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), -2);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 3);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x510);
}

TEST(execute_next, BLT) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_blt(Rv_register_id::x2, Rv_register_id::x3, 0x10);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x2, 2);
	hart.set_register(Rv_register_id::x3, 3);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 2);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 3);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x510);
}

TEST(execute_next, BLTU) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_bltu(Rv_register_id::x2, Rv_register_id::x3, 0x10);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x2, 2);
	hart.set_register(Rv_register_id::x3, -3);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 2);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), -3);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x510);
}

TEST(execute_next, BNE) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_bne(Rv_register_id::x2, Rv_register_id::x3, 0x10);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x2, 1);
	hart.set_register(Rv_register_id::x3, 2);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 2);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x510);
}

TEST(execute_next, EBREAK) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_ebreak();
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	EXPECT_THROW_EX(hart.execute_next(), "ebreak");
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x500);
}

TEST(execute_next, ECALL) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_ecall();
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	EXPECT_THROW_EX(hart.execute_next(), "ecall");
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x500);
}

TEST(execute_next, FENCE) {

	// FENCE is a NOP in this implementation

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_fence(Rv_register_id::x0, Rv_register_id::x0, Rv_itype_imm::from_unsigned(0));
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, JAL) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_jal(Rv_register_id::x1, Rv_jtype_imm::from_offset(0x20));
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x520);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0x504);
}

TEST(execute_next, JALR) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_jalr(Rv_register_id::x1, Rv_register_id::x2, Rv_itype_imm::from_signed(0x20));
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x2, 0x600);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x620);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0x504);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x600);
}

TEST(execute_next, LB) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_lb(Rv_register_id::x2, Rv_register_id::x3, 0);
	memory.write_32(0x500, instruction);
	memory.write_8(0x600, 0x20);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 0x600);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x20);
}

TEST(execute_next, LBU) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_lb(Rv_register_id::x2, Rv_register_id::x3, 0);
	memory.write_32(0x500, instruction);
	memory.write_8(0x600, 0x20);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 0x600);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x20);
}

TEST(execute_next, LH) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_lh(Rv_register_id::x2, Rv_register_id::x3, 0);
	memory.write_32(0x500, instruction);
	memory.write_8(0x600, 0x20);
	memory.write_8(0x601, 0x30);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 0x600);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x3020);
}

TEST(execute_next, LHU) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_lhu(Rv_register_id::x2, Rv_register_id::x3, 0);
	memory.write_32(0x500, instruction);
	memory.write_8(0x600, 0x20);
	memory.write_8(0x601, 0x30);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 0x600);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x3020);
}

TEST(execute_next, LW) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_lw(Rv_register_id::x2, Rv_register_id::x3, 0);
	memory.write_32(0x500, instruction);
	memory.write_32(0x600, 0x40302010);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 0x600);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x40302010);
}

TEST(execute_next, LUI) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_lui(Rv_register_id::x2, 0b1111);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0b1111'0000'0000'0000);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, NOP) {

	// NOP isn't an actual instruction, it's encoded as ADDI x0, x0, 0.

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_addi(Rv_register_id::x0, Rv_register_id::x0, 0);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x0), 0);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, OR) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_or(Rv_register_id::x2, Rv_register_id::x3, Rv_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 0b1010);
	hart.set_register(Rv_register_id::x4, 0b1001);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0b1011);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0b1010);
	EXPECT_EQ(hart.get_register(Rv_register_id::x4), 0b1001);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, ORI) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_ori(Rv_register_id::x2, Rv_register_id::x3, 0b010);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 0b011);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0b011);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0b011);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, SB) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_sb(Rv_register_id::x2, Rv_register_id::x3, 0x10);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x2, 0x600);
	hart.set_register(Rv_register_id::x3, 0x30);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x600);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0x30);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
	EXPECT_EQ(memory.read_8(0x610), 0x30);
}

TEST(execute_next, SH) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_sh(Rv_register_id::x2, Rv_register_id::x3, 0x10);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x2, 0x600);
	hart.set_register(Rv_register_id::x3, 0x2030);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x600);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0x2030);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
	EXPECT_EQ(memory.read_16(0x610), 0x2030);
}

TEST(execute_next, SLL) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_sll(Rv_register_id::x2, Rv_register_id::x3, Rv_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 0b1101);
	hart.set_register(Rv_register_id::x4, 3);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0b1101000);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0b1101);
	EXPECT_EQ(hart.get_register(Rv_register_id::x4), 3);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, SLLI) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_slli(Rv_register_id::x2, Rv_register_id::x3, 2);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 0b11);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0b1100);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0b0011);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, SLT) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_slt(Rv_register_id::x2, Rv_register_id::x3, Rv_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, -1);
	hart.set_register(Rv_register_id::x4, 1);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), -1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x4), 1);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, SLTI) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_slti(Rv_register_id::x2, Rv_register_id::x3, 5);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 3);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 3);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, SLTIU) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_sltiu(Rv_register_id::x2, Rv_register_id::x3, 5);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, -3);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), -3);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, SLTU) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_sltu(Rv_register_id::x2, Rv_register_id::x3, Rv_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 4);
	hart.set_register(Rv_register_id::x4, 10);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x4), 10);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, SRA) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_sra(Rv_register_id::x2, Rv_register_id::x3, Rv_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 0xFF000000);
	hart.set_register(Rv_register_id::x4, 8);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0xFFFF0000);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0xFF000000);
	EXPECT_EQ(hart.get_register(Rv_register_id::x4), 8);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, SRAI) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_srai(Rv_register_id::x2, Rv_register_id::x3, 2);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, -1);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), -1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), -1);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, SRL) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_srl(Rv_register_id::x2, Rv_register_id::x3, Rv_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 0xFF000000);
	hart.set_register(Rv_register_id::x4, 8);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x00FF0000);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0xFF000000);
	EXPECT_EQ(hart.get_register(Rv_register_id::x4), 8);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, SRLI) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_srli(Rv_register_id::x2, Rv_register_id::x3, 4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 0xFFFFFFFF);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x0FFFFFFF);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), -1);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, SUB) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_sub(Rv_register_id::x2, Rv_register_id::x3, Rv_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 4);
	hart.set_register(Rv_register_id::x4, 10);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), -6);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x4), 10);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, SW) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_sw(Rv_register_id::x2, Rv_register_id::x3, 0x10);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x2, 0x600);
	hart.set_register(Rv_register_id::x3, 0x40302010);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x600);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0x40302010);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
	EXPECT_EQ(memory.read_32(0x610), 0x40302010);
}

TEST(execute_next, XOR) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_xor(Rv_register_id::x2, Rv_register_id::x3, Rv_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 0b1010);
	hart.set_register(Rv_register_id::x4, 0b1001);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0b0011);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0b1010);
	EXPECT_EQ(hart.get_register(Rv_register_id::x4), 0b1001);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

TEST(execute_next, XORI) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_xori(Rv_register_id::x2, Rv_register_id::x3, 0b010);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv_register_id::pc, 0x500);
	hart.set_register(Rv_register_id::x3, 0b110);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0b100);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0b110);
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x504);
}

/* --------------------------------------------------------
ADD
-------------------------------------------------------- */

TEST(execute_add, DifferentRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 14);
	hart.execute_add(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 18);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 14);
}

TEST(execute_add, SameRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 4);
	hart.execute_add(Rv_register_id::x1, Rv_register_id::x1, Rv_register_id::x1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 8);
}

/* --------------------------------------------------------
ADDI
-------------------------------------------------------- */

TEST(execute_addi, AddZeroToX0) {

	// Add zero and store in zero register
	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.execute_addi(Rv_register_id::x0, Rv_register_id::x1, Rv_itype_imm::from_signed(0));
	EXPECT_EQ(hart.get_register(Rv_register_id::x0), 0);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);
}

TEST(execute_addi, AddNonZeroToX0) {

	// Add non-zero and store in zero register (zero register can't be changed)
	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.execute_addi(Rv_register_id::x0, Rv_register_id::x1, Rv_itype_imm::from_signed(21));
	EXPECT_EQ(hart.get_register(Rv_register_id::x0), 0);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);
}

TEST(execute_addi, AddWithSameDestAndSrc) {

	// Use same register for rd and rs1
	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.execute_addi(Rv_register_id::x1, Rv_register_id::x1, Rv_itype_imm::from_signed(21));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 21);

	// Repeat with a non-zero value in rs1
	hart.set_register(Rv_register_id::x1, 10);
	hart.execute_addi(Rv_register_id::x1, Rv_register_id::x1, Rv_itype_imm::from_signed(21));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 31);
}

TEST(execute_addi, AddWithDiffDestAndSrc) {

	// Use different register for rd and rs1
	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.execute_addi(Rv_register_id::x1, Rv_register_id::x2, Rv_itype_imm::from_signed(21));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 21);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0);

	// Repeat with a non-zero value in rs1
	hart.set_register(Rv_register_id::x2, 10);
	hart.execute_addi(Rv_register_id::x1, Rv_register_id::x2, Rv_itype_imm::from_signed(21));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 31);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 10);
}

TEST(execute_addi, OverflowIgnored) {

	// Arithmetic overflow is ignored and the result is simply the low XLEN bits of the result.
	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x5, 0xFFFFFFFF);
	hart.execute_addi(Rv_register_id::x5, Rv_register_id::x5, Rv_itype_imm::from_signed(1));
	EXPECT_EQ(hart.get_register(Rv_register_id::x5), 0);
}

TEST(execute_addi, NOP) {

	// NOP is encoded as ADDI x0, x0, 0
	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.execute_addi(Rv_register_id::x0, Rv_register_id::x0, Rv_itype_imm::from_signed(0));
	EXPECT_EQ(hart.get_register(Rv_register_id::x0), 0);
}

/* --------------------------------------------------------
AND
-------------------------------------------------------- */

TEST(execute_and, DifferentRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0b1101);
	hart.set_register(Rv_register_id::x3, 0b1011);
	hart.execute_and(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0b1001);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0b1101);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0b1011);
}

TEST(execute_and, SameRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 0b1110);
	hart.execute_and(Rv_register_id::x1, Rv_register_id::x1, Rv_register_id::x1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0b1110);
}

/* --------------------------------------------------------
ANDI
-------------------------------------------------------- */

TEST(execute_andi, NoMatchingBits) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0b01000000000000110);
	hart.execute_andi(Rv_register_id::x1, Rv_register_id::x2, Rv_itype_imm::from_signed(0b1001));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);
}

TEST(execute_andi, MatchingBit) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0b01000000000000110);
	hart.execute_andi(Rv_register_id::x1, Rv_register_id::x2, Rv_itype_imm::from_signed(0b0100));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0b0100);
}

/* --------------------------------------------------------
AUIPC
-------------------------------------------------------- */

TEST(execute_auipc, ValidInstruction) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 0x200);

	// Only 20-bit immediate, ensure low 12 bits are ignored
	hart.execute_auipc(Rv_register_id::x1, Rv_utype_imm::from_decoded(0b0101'1111'0101'1111'0101'1111'1111'1111));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0x200 + 0b0101'1111'0101'1111'0101'0000'0000'0000);
}

/* --------------------------------------------------------
BEQ
-------------------------------------------------------- */

TEST(execute_beq, BranchTakenWithPositiveOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_beq(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_beq, BranchTakenWithNegativeOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 464);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_beq(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 400);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_beq, BranchNotTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 464);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 7);
	hart.execute_beq(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 468); // PC just gets advanced to next instruction
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 7);
}

TEST(execute_beq, AddressMisalignedAndBranchTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 4);

	EXPECT_THROW_EX(hart.execute_beq(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(2)), "instruction-address-misaligned");
}

TEST(execute_beq, AddressMisalignedButBranchNotTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 7);
	hart.execute_beq(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(2));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 404); // PC just gets advanced to next instruction
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 7);
}

/* --------------------------------------------------------
BGE
-------------------------------------------------------- */

TEST(execute_bge, BranchTakenBecauseGreaterThan) {

	// rs1 > rs2, branch taken

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 5);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_bge(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 5);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);

	// Ensure signed comparison

	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 5);
	hart.set_register(Rv_register_id::x3, -4);
	hart.execute_bge(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 5);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), -4);
}

TEST(execute_bge, BranchTakenBecauseEqualTo) {

	// rs1 == rs2, branch taken

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_bge(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_bge, BranchNotTaken) {

	// rs1 < rs2, so PC gets advanced to next instruction

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 464);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 5);
	hart.execute_bge(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 468);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 5);

	// Ensure signed comparison

	hart.set_register(Rv_register_id::pc, 464);
	hart.set_register(Rv_register_id::x2, -5);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_bge(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 468);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), -5);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_bge, AddressMisalignedAndBranchTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 3);

	EXPECT_THROW_EX(hart.execute_bge(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(2)), "instruction-address-misaligned");
}

TEST(execute_bge, AddressMisalignedButBranchNotTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 5);
	hart.execute_bge(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(2));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 404);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 5);
}

/* --------------------------------------------------------
BGEU
-------------------------------------------------------- */

TEST(execute_bgeu, BranchTakenBecauseGreaterThan) {

	// rs1 > rs2, branch taken

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 5);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_bgeu(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 5);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);

	// Ensure UNSIGNED comparison
	// -5 is less than 4, but -5 will interpreted as unsigned

	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, -5);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_bgeu(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), -5);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_bgeu, BranchTakenBecauseEqualTo) {

	// rs1 == rs2, branch taken

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_bgeu(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_bgeu, BranchNotTaken) {

	// rs1 < rs2, so PC gets advanced to next instruction

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 464);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 5);
	hart.execute_bgeu(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 468);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 5);

	// Ensure UNSIGNED comparison
	// 4 is greater than -5, but -5 will interpreted as unsigned

	hart.set_register(Rv_register_id::pc, 464);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, -5);
	hart.execute_bgeu(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 468);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), -5);
}

TEST(execute_bgeu, AddressMisalignedAndBranchTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 3);

	EXPECT_THROW_EX(hart.execute_bgeu(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(2)), "instruction-address-misaligned");
}

TEST(execute_bgeu, AddressMisalignedButBranchNotTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 5);
	hart.execute_bgeu(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(2));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 404);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 5);
}

/* --------------------------------------------------------
BLT
-------------------------------------------------------- */

TEST(execute_blt, BranchTaken) {

	// rs1 < rs2, branch taken

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 5);
	hart.execute_blt(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 5);

	// Ensure signed comparison

	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, -4);
	hart.set_register(Rv_register_id::x3, 5);
	hart.execute_blt(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), -4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 5);
}

TEST(execute_blt, BranchNotTakenBecauseEqualTo) {

	// rs1 == rs2, so PC gets advanced to next instruction

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_blt(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 404);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_blt, BranchNotTakenBecauseGreaterThan) {

	// rs1 > rs2, so PC gets advanced to next instruction

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 464);
	hart.set_register(Rv_register_id::x2, 5);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_blt(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 468);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 5);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);

	// Ensure signed comparison

	hart.set_register(Rv_register_id::pc, 464);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, -5);
	hart.execute_blt(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 468);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), -5);
}

TEST(execute_blt, AddressMisalignedAndBranchTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 3);
	hart.set_register(Rv_register_id::x3, 4);

	EXPECT_THROW_EX(hart.execute_blt(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(2)), "instruction-address-misaligned");
}

TEST(execute_blt, AddressMisalignedButBranchNotTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 3);
	hart.execute_blt(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(2));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 404);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 3);
}

/* --------------------------------------------------------
BLTU
-------------------------------------------------------- */

TEST(execute_bltu, BranchTaken) {

	// rs1 < rs2, branch taken

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 5);
	hart.execute_bltu(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 5);

	// Ensure UNSIGNED comparison
	// 5 is greater than -4, but -4 is interpreted as unsigned

	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 5);
	hart.set_register(Rv_register_id::x3, -4);
	hart.execute_bltu(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 5);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), -4);
}

TEST(execute_bltu, BranchNotTakenBecauseEqualTo) {

	// rs1 == rs2, so PC gets advanced to next instruction

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_bltu(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 404);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_bltu, BranchNotTakenBecauseGreaterThan) {

	// rs1 > rs2, so PC gets advanced to next instruction

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 464);
	hart.set_register(Rv_register_id::x2, 5);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_bltu(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 468);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 5);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);

	// Ensure UNSIGNED comparison
	// -5 is less than 4, but -5 is interpreted as unsigned

	hart.set_register(Rv_register_id::pc, 464);
	hart.set_register(Rv_register_id::x2, -5);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_bltu(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 468);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), -5);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_bltu, AddressMisalignedAndBranchTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 3);
	hart.set_register(Rv_register_id::x3, 4);

	EXPECT_THROW_EX(hart.execute_bltu(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(2)), "instruction-address-misaligned");
}

TEST(execute_bltu, AddressMisalignedButBranchNotTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 3);
	hart.execute_bltu(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(2));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 404);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 3);
}

/* --------------------------------------------------------
BNE
-------------------------------------------------------- */

TEST(execute_bne, BranchTakenWithPositiveOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 5);
	hart.execute_bne(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 5);
}

TEST(execute_bne, BranchTakenWithNegativeOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 464);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 5);
	hart.execute_bne(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 400);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 5);
}

TEST(execute_bne, BranchNotTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 464);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_bne(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 468); // PC just gets advanced to next instruction
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_bne, AddressMisalignedAndBranchTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 7);

	EXPECT_THROW_EX(hart.execute_bne(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(2)), "instruction-address-misaligned");
}

TEST(execute_bne, AddressMisalignedButBranchNotTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 400);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_bne(Rv_register_id::x2, Rv_register_id::x3, Rv_btype_imm::from_offset(2));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 404); // PC just gets advanced to next instruction
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

/* --------------------------------------------------------
JAL
-------------------------------------------------------- */

TEST(execute_jal, PositiveOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 0x40);
	hart.execute_jal(Rv_register_id::x1, Rv_jtype_imm::from_offset(0x20));
	
	// PC is set to the jump target (PC + Offset)
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x60);

	// RD is set to instruction after the jump instruction (PC + 4)
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0x44);
}

TEST(execute_jal, PositiveOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 0xFFFFFFFC);
	hart.execute_jal(Rv_register_id::x1, Rv_jtype_imm::from_offset(8));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);
}

TEST(execute_jal, NegativeOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 0x40);
	hart.execute_jal(Rv_register_id::x1, Rv_jtype_imm::from_offset(-0x20));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x20);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0x44);
}

TEST(execute_jal, NegativeOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 0x40);
	hart.execute_jal(Rv_register_id::x1, Rv_jtype_imm::from_offset(-0x60));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), -0x20);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0x44);
}

TEST(execute_jal, NoOffset) {

	// This would result in an infinite loop, but I don't see anything that technically prohibits this

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 0x40);
	hart.execute_jal(Rv_register_id::x1, Rv_jtype_imm::from_offset(0));
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x40);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0x44);
}

TEST(execute_jal, TargetAddressMisaligned) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	const auto expected_message = "instruction-address-misaligned";
	
	hart.set_register(Rv_register_id::pc, 0x41);
	EXPECT_THROW_EX(hart.execute_jal(Rv_register_id::x1, Rv_jtype_imm::from_offset(0)), expected_message);

	hart.set_register(Rv_register_id::pc, 0x42);
	EXPECT_THROW_EX(hart.execute_jal(Rv_register_id::x1, Rv_jtype_imm::from_offset(0)), expected_message);
	
	hart.set_register(Rv_register_id::pc, 0x43);
	EXPECT_THROW_EX(hart.execute_jal(Rv_register_id::x1, Rv_jtype_imm::from_offset(0)), expected_message);
}

/* --------------------------------------------------------
JALR
-------------------------------------------------------- */

TEST(execute_jalr, PositiveOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 0x40);
	hart.set_register(Rv_register_id::x2, 0x80);
	hart.execute_jalr(Rv_register_id::x1, Rv_register_id::x2, Rv_itype_imm::from_signed(0x10));

	// PC is set to RS1 + Offset
	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x90);

	// RD is set to instruction after the jump instruction (PC + 4)
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0x44);
}

TEST(execute_jalr, PositiveOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 0x40);
	hart.set_register(Rv_register_id::x2, 0xFFFFFF00);
	hart.execute_jalr(Rv_register_id::x1, Rv_register_id::x2, Rv_itype_imm::from_signed(0x104));

	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0x44);
}

TEST(execute_jalr, NegativeOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 0x40);
	hart.set_register(Rv_register_id::x2, 0x80);
	hart.execute_jalr(Rv_register_id::x1, Rv_register_id::x2, Rv_itype_imm::from_signed(-0x10));

	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x70);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0x44);
}

TEST(execute_jalr, NegativeOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 0x40);
	hart.set_register(Rv_register_id::x2, 0x4);
	hart.execute_jalr(Rv_register_id::x1, Rv_register_id::x2, Rv_itype_imm::from_signed(-0x104));

	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0xFFFFFF00);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0x44);
}

TEST(execute_jalr, LeastSigBitSetToZero) {

	// After computing the target address, the least significant bit of the target address
	// is set to zero.

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::pc, 0x40);
	hart.set_register(Rv_register_id::x2, 0x4);
	hart.execute_jalr(Rv_register_id::x1, Rv_register_id::x2, Rv_itype_imm::from_signed(1));

	// 4 + 1 = 5 == 0b101
	// Set LSB to 0 and you get 0b100

	EXPECT_EQ(hart.get_register(Rv_register_id::pc), 0x4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0x44);
}

TEST(execute_jalr, TargetAddressMisaligned) {

	// Since JALR sets LSB to 0 of target address, the only case
	// that can trigger this is on a two byte boundary that isn't
	// also a four byte boundary.

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	const auto expected_message = "instruction-address-misaligned";
	hart.set_register(Rv_register_id::x2, 0x52);
	EXPECT_THROW_EX(hart.execute_jalr(Rv_register_id::x1, Rv_register_id::x2, Rv_itype_imm::from_signed(0)), expected_message);
}

/* --------------------------------------------------------
LB
-------------------------------------------------------- */

TEST(execute_lb, PositiveOffset) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(96, 1);
	memory.write_8(97, 2);
	memory.write_8(98, 3);
	memory.write_8(99, 4);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 64);
	hart.execute_lb(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(32));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 64);
}

TEST(execute_lb, PositiveOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(4, 10);
	memory.write_8(5, 20);
	memory.write_8(6, 30);
	memory.write_8(7, 40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 0xFFFFFFFC);
	hart.execute_lb(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(8));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 10);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0xFFFFFFFC);
}

TEST(execute_lb, NegativeOffset) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(32, 10);
	memory.write_8(33, 20);
	memory.write_8(34, 30);
	memory.write_8(35, 40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 64);
	hart.execute_lb(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(-32));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 10);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 64);
}

TEST(execute_lb, NegativeOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(0xFFFFFFFC, 10);
	memory.write_8(0xFFFFFFFD, 20);
	memory.write_8(0xFFFFFFFE, 30);
	memory.write_8(0xFFFFFFFF, 40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_lb(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(-8));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 10);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_lb, ZeroOffset) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(4, 10);
	memory.write_8(5, 20);
	memory.write_8(6, 30);
	memory.write_8(7, 40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 6);
	hart.execute_lb(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(0));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 30);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 6);
}

TEST(execute_lb, MisalignedAccess) {

	// Address 5 is not on a 4 or 2 byte boundary, but that is allowed

	auto memory = Simple_memory_subsystem();
	memory.write_8(4, 10);
	memory.write_8(5, 20);
	memory.write_8(6, 30);
	memory.write_8(7, 40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_lb(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(1));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 20);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_lb, SignExtended) {

	// Ensures the result is signed extended

	auto memory = Simple_memory_subsystem();
	memory.write_8(4, 0b1000'0000);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_lb(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(0));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), -1 << 7);
}

/* --------------------------------------------------------
LBU
-------------------------------------------------------- */

TEST(execute_lbu, PositiveOffset) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(96, 1);
	memory.write_8(97, 2);
	memory.write_8(98, 3);
	memory.write_8(99, 4);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 64);
	hart.execute_lbu(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(32));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 64);
}

TEST(execute_lbu, PositiveOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(4, 10);
	memory.write_8(5, 20);
	memory.write_8(6, 30);
	memory.write_8(7, 40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 0xFFFFFFFC);
	hart.execute_lbu(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(8));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 10);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0xFFFFFFFC);
}

TEST(execute_lbu, NegativeOffset) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(32, 10);
	memory.write_8(33, 20);
	memory.write_8(34, 30);
	memory.write_8(35, 40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 64);
	hart.execute_lbu(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(-32));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 10);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 64);
}

TEST(execute_lbu, NegativeOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(0xFFFFFFFC, 10);
	memory.write_8(0xFFFFFFFD, 20);
	memory.write_8(0xFFFFFFFE, 30);
	memory.write_8(0xFFFFFFFF, 40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_lbu(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(-8));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 10);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_lbu, ZeroOffset) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(4, 10);
	memory.write_8(5, 20);
	memory.write_8(6, 30);
	memory.write_8(7, 40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 6);
	hart.execute_lbu(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(0));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 30);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 6);
}

TEST(execute_lbu, MisalignedAccess) {

	// Address 5 is not on a 4 or 2 byte boundary, but that is allowed

	auto memory = Simple_memory_subsystem();
	memory.write_8(4, 10);
	memory.write_8(5, 20);
	memory.write_8(6, 30);
	memory.write_8(7, 40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_lbu(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(1));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 20);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_lbu, NotSignExtended) {

	// Ensures the result is NOT signed extended

	auto memory = Simple_memory_subsystem();
	memory.write_8(4, 0b1000'0000);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_lbu(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(0));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0b1000'0000);
}

/* --------------------------------------------------------
LH
-------------------------------------------------------- */

TEST(execute_lh, PositiveOffset) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(96, 0x10);
	memory.write_8(97, 0x20);
	memory.write_8(98, 0x30);
	memory.write_8(99, 0x40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 64);
	hart.execute_lh(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(32));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x2010);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 64);
}

TEST(execute_lh, PositiveOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(4, 0x10);
	memory.write_8(5, 0x20);
	memory.write_8(6, 0x30);
	memory.write_8(7, 0x40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 0xFFFFFFFC);
	hart.execute_lh(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(8));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x2010);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0xFFFFFFFC);
}

TEST(execute_lh, NegativeOffset) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(32, 0x10);
	memory.write_8(33, 0x20);
	memory.write_8(34, 0x30);
	memory.write_8(35, 0x40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 64);
	hart.execute_lh(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(-32));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x2010);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 64);
}

TEST(execute_lh, NegativeOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(0xFFFFFFFC, 0x10);
	memory.write_8(0xFFFFFFFD, 0x20);
	memory.write_8(0xFFFFFFFE, 0x30);
	memory.write_8(0xFFFFFFFF, 0x40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_lh(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(-8));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x2010);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_lh, ZeroOffset) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(4, 0x10);
	memory.write_8(5, 0x20);
	memory.write_8(6, 0x30);
	memory.write_8(7, 0x40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 6);
	hart.execute_lh(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(0));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x4030);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 6);
}

TEST(execute_lh, MisalignedAccess) {

	// Address 5 is not on a 4 or 2 byte boundary, but that is allowed

	auto memory = Simple_memory_subsystem();
	memory.write_8(4, 0x10);
	memory.write_8(5, 0x20);
	memory.write_8(6, 0x30);
	memory.write_8(7, 0x40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_lh(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(1));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x3020);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_lh, SignExtended) {

	// Ensures the result is signed extended

	auto memory = Simple_memory_subsystem();
	memory.write_8(4, 0b0000'0000);
	memory.write_8(5, 0b1000'0000);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_lh(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(0));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), -1 << 15);
}

/* --------------------------------------------------------
LHU
-------------------------------------------------------- */

TEST(execute_lhu, PositiveOffset) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(96, 0x10);
	memory.write_8(97, 0x20);
	memory.write_8(98, 0x30);
	memory.write_8(99, 0x40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 64);
	hart.execute_lhu(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(32));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x2010);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 64);
}

TEST(execute_lhu, PositiveOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(4, 0x10);
	memory.write_8(5, 0x20);
	memory.write_8(6, 0x30);
	memory.write_8(7, 0x40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 0xFFFFFFFC);
	hart.execute_lhu(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(8));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x2010);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0xFFFFFFFC);
}

TEST(execute_lhu, NegativeOffset) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(32, 0x10);
	memory.write_8(33, 0x20);
	memory.write_8(34, 0x30);
	memory.write_8(35, 0x40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 64);
	hart.execute_lhu(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(-32));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x2010);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 64);
}

TEST(execute_lhu, NegativeOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(0xFFFFFFFC, 0x10);
	memory.write_8(0xFFFFFFFD, 0x20);
	memory.write_8(0xFFFFFFFE, 0x30);
	memory.write_8(0xFFFFFFFF, 0x40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_lhu(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(-8));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x2010);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_lhu, ZeroOffset) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(4, 0x10);
	memory.write_8(5, 0x20);
	memory.write_8(6, 0x30);
	memory.write_8(7, 0x40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 6);
	hart.execute_lhu(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(0));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x4030);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 6);
}

TEST(execute_lhu, MisalignedAccess) {

	// Address 5 is not on a 4 or 2 byte boundary, but that is allowed

	auto memory = Simple_memory_subsystem();
	memory.write_8(4, 0x10);
	memory.write_8(5, 0x20);
	memory.write_8(6, 0x30);
	memory.write_8(7, 0x40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_lhu(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(1));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x3020);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_lhu, SignExtended) {

	// Ensures the result is NOT signed extended

	auto memory = Simple_memory_subsystem();
	memory.write_8(4, 0b0000'0000);
	memory.write_8(5, 0b1000'0000);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_lhu(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(0));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 1 << 15);
}

/* --------------------------------------------------------
LW
-------------------------------------------------------- */

TEST(execute_lw, PositiveOffset) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(96, 0x10);
	memory.write_8(97, 0x20);
	memory.write_8(98, 0x30);
	memory.write_8(99, 0x40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 64);
	hart.execute_lw(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(32));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x40302010);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 64);
}

TEST(execute_lw, PositiveOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(4, 0x10);
	memory.write_8(5, 0x20);
	memory.write_8(6, 0x30);
	memory.write_8(7, 0x40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 0xFFFFFFFC);
	hart.execute_lw(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(8));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x40302010);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0xFFFFFFFC);
}

TEST(execute_lw, NegativeOffset) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(32, 0x10);
	memory.write_8(33, 0x20);
	memory.write_8(34, 0x30);
	memory.write_8(35, 0x40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 64);
	hart.execute_lw(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(-32));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x40302010);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 64);
}

TEST(execute_lw, NegativeOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(0xFFFFFFFC, 0x10);
	memory.write_8(0xFFFFFFFD, 0x20);
	memory.write_8(0xFFFFFFFE, 0x30);
	memory.write_8(0xFFFFFFFF, 0x40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_lw(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(-8));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x40302010);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_lw, ZeroOffset) {

	auto memory = Simple_memory_subsystem();
	memory.write_8(4, 0x10);
	memory.write_8(5, 0x20);
	memory.write_8(6, 0x30);
	memory.write_8(7, 0x40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_lw(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(0));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x40302010);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_lw, MisalignedAccess) {

	// Address 5 is not on a 4 or 2 byte boundary, but that is allowed

	auto memory = Simple_memory_subsystem();
	memory.write_8(5, 0x10);
	memory.write_8(6, 0x20);
	memory.write_8(7, 0x30);
	memory.write_8(8, 0x40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_lw(Rv_register_id::x2, Rv_register_id::x3, Rv_itype_imm::from_signed(1));
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x40302010);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

/* --------------------------------------------------------
LUI
-------------------------------------------------------- */

TEST(execute_lui, ValidInstruction) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	// Only 20-bit immediate, ensure low 12 bits are cleared
	hart.execute_lui(Rv_register_id::x1, Rv_utype_imm::from_decoded(0b0101'1111'0101'1111'0101'1111'1111'1111));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0b0101'1111'0101'1111'0101'0000'0000'0000);
}

/* --------------------------------------------------------
OR
-------------------------------------------------------- */

TEST(execute_or, DifferentRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0b1101);
	hart.set_register(Rv_register_id::x3, 0b1011);
	hart.execute_or(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0b1111);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0b1101);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0b1011);
}

TEST(execute_or, SameRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 0b1110);
	hart.execute_or(Rv_register_id::x1, Rv_register_id::x1, Rv_register_id::x1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0b1110);
}

/* --------------------------------------------------------
ORI
-------------------------------------------------------- */

TEST(execute_ori, ValidInstruction) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0b01000000000000110);
	hart.execute_ori(Rv_register_id::x1, Rv_register_id::x2, Rv_itype_imm::from_signed(0b1001));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0b01000000000001111);
}

/* --------------------------------------------------------
SB
-------------------------------------------------------- */

TEST(execute_sb, PositiveOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 0x100);
	hart.set_register(Rv_register_id::x2, 0x50);
	hart.execute_sb(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(0x10));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0x100);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x50);
	EXPECT_EQ(memory.read_8(0x110), 0x50);
}

TEST(execute_sb, PositiveOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 0xFFFFFFFC);
	hart.set_register(Rv_register_id::x2, 0x50);
	hart.execute_sb(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(8));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0xFFFFFFFC);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x50);
	EXPECT_EQ(memory.read_8(4), 0x50);
}

TEST(execute_sb, NegativeOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 64);
	hart.set_register(Rv_register_id::x2, 0x50);
	hart.execute_sb(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(-32));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 64);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x50);
	EXPECT_EQ(memory.read_8(32), 0x50);
}

TEST(execute_sb, NegativeOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 4);
	hart.set_register(Rv_register_id::x2, 0x50);
	hart.execute_sb(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(-8));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x50);
	EXPECT_EQ(memory.read_8(0xFFFFFFFC), 0x50);
}

TEST(execute_sb, ZeroOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 6);
	hart.set_register(Rv_register_id::x2, 0x50);
	hart.execute_sb(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(0));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 6);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x50);
	EXPECT_EQ(memory.read_8(6), 0x50);
}

TEST(execute_sb, MisalignedAccess) {

	// Address 5 is not on a 4 or 2 byte boundary, but that is allowed

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 4);
	hart.set_register(Rv_register_id::x2, 0x50);
	hart.execute_sb(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(1));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x50);
	EXPECT_EQ(memory.read_8(5), 0x50);
}

TEST(execute_sb, HighBitsOfRegisterIgnored) {

	// Only the low 8 bits of the source register are stored

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 4);
	hart.set_register(Rv_register_id::x2, -1 << 7);
	hart.execute_sb(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(0));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), -1 << 7);
	EXPECT_EQ(memory.read_8(4), 0b1000'0000);
}

/* --------------------------------------------------------
SH
-------------------------------------------------------- */

TEST(execute_sh, PositiveOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 0x100);
	hart.set_register(Rv_register_id::x2, 0x4050);
	hart.execute_sh(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(0x10));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0x100);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x4050);
	EXPECT_EQ(memory.read_16(0x110), 0x4050);
}

TEST(execute_sh, PositiveOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 0xFFFFFFFC);
	hart.set_register(Rv_register_id::x2, 0x4050);
	hart.execute_sh(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(8));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0xFFFFFFFC);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x4050);
	EXPECT_EQ(memory.read_16(4), 0x4050);
}

TEST(execute_sh, NegativeOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 64);
	hart.set_register(Rv_register_id::x2, 0x4050);
	hart.execute_sh(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(-32));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 64);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x4050);
	EXPECT_EQ(memory.read_16(32), 0x4050);
}

TEST(execute_sh, NegativeOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 4);
	hart.set_register(Rv_register_id::x2, 0x4050);
	hart.execute_sh(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(-8));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x4050);
	EXPECT_EQ(memory.read_16(0xFFFFFFFC), 0x4050);
}

TEST(execute_sh, ZeroOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 6);
	hart.set_register(Rv_register_id::x2, 0x4050);
	hart.execute_sh(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(0));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 6);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x4050);
	EXPECT_EQ(memory.read_16(6), 0x4050);
}

TEST(execute_sh, MisalignedAccess) {

	// Address 5 is not on a 4 or 2 byte boundary, but that is allowed

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 4);
	hart.set_register(Rv_register_id::x2, 0x4050);
	hart.execute_sh(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(1));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x4050);
	EXPECT_EQ(memory.read_16(5), 0x4050);
}

TEST(execute_sh, HighBitsOfRegisterIgnored) {

	// Only the low 8 bits of the source register are stored

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 4);
	hart.set_register(Rv_register_id::x2, -1 << 15);
	hart.execute_sh(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(0));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), -1 << 15);
	EXPECT_EQ(memory.read_16(4), 1 << 15);
}

/* --------------------------------------------------------
SLL
-------------------------------------------------------- */

TEST(execute_sll, DifferentRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 1);
	hart.set_register(Rv_register_id::x3, 4);
	hart.execute_sll(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0b10000);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 4);
}

TEST(execute_sll, SameRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 0b10);
	hart.execute_sll(Rv_register_id::x1, Rv_register_id::x1, Rv_register_id::x1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0b1000);
}

TEST(execute_sll, MoreThanFiveBitsSetInRs2) {

	// Shift amount is in RS2, but only the low 5 bits are used.

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 1);
	hart.set_register(Rv_register_id::x3, 0b100001);
	hart.execute_sll(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 2);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0b100001);
}

TEST(execute_sll, ShiftOutOfHighBit) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0xFFFFFFFF);
	hart.set_register(Rv_register_id::x3, 8);
	hart.execute_sll(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0xFFFFFF00);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0xFFFFFFFF);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 8);
}

/* --------------------------------------------------------
SLLI
-------------------------------------------------------- */

TEST(execute_slli, ValidInstruction) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0b0100'0000'0000'0000'0000'0000'0000'0011);
	hart.execute_slli(Rv_register_id::x1, Rv_register_id::x2, Rv_itype_imm::from_signed(2));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0b1100);
}

/* --------------------------------------------------------
SLT
-------------------------------------------------------- */

TEST(execute_slt, LessThan) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 2);
	hart.set_register(Rv_register_id::x3, 3);
	hart.execute_slt(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 1);

	hart.set_register(Rv_register_id::x2, -2);
	hart.set_register(Rv_register_id::x3, 3);
	hart.execute_slt(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 1);

	hart.set_register(Rv_register_id::x2, -2);
	hart.set_register(Rv_register_id::x3, -1);
	hart.execute_slt(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 1);
}

TEST(execute_slt, Equal) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 2);
	hart.set_register(Rv_register_id::x3, 2);
	hart.execute_slt(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);

	hart.set_register(Rv_register_id::x2, -2);
	hart.set_register(Rv_register_id::x3, -2);
	hart.execute_slt(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);
}

TEST(execute_slt, GreaterThan) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 3);
	hart.set_register(Rv_register_id::x3, 2);
	hart.execute_slt(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);

	hart.set_register(Rv_register_id::x2, 2);
	hart.set_register(Rv_register_id::x3, -3);
	hart.execute_slt(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);

	hart.set_register(Rv_register_id::x2, -3);
	hart.set_register(Rv_register_id::x3, -4);
	hart.execute_slt(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);
}

/* --------------------------------------------------------
SLTI
-------------------------------------------------------- */

TEST(execute_slti, SrcEqualsImm) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x5, 2);
	hart.execute_slti(Rv_register_id::x1, Rv_register_id::x5, Rv_itype_imm::from_signed(2));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);

	hart.set_register(Rv_register_id::x5, -3);
	hart.execute_slti(Rv_register_id::x1, Rv_register_id::x5, Rv_itype_imm::from_signed(-3));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);
}

TEST(execute_slti, SrcLessThanImm) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x5, 1);
	hart.execute_slti(Rv_register_id::x1, Rv_register_id::x5, Rv_itype_imm::from_signed(2));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 1);

	hart.set_register(Rv_register_id::x5, -5);
	hart.execute_slti(Rv_register_id::x1, Rv_register_id::x5, Rv_itype_imm::from_signed(-3));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 1);

	hart.set_register(Rv_register_id::x5, -5);
	hart.execute_slti(Rv_register_id::x1, Rv_register_id::x5, Rv_itype_imm::from_signed(6));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 1);
}

TEST(execute_slti, SrcGreaterThanImm) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x5, 3);
	hart.execute_slti(Rv_register_id::x1, Rv_register_id::x5, Rv_itype_imm::from_signed(2));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);

	hart.set_register(Rv_register_id::x5, -1);
	hart.execute_slti(Rv_register_id::x1, Rv_register_id::x5, Rv_itype_imm::from_signed(-3));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);

	hart.set_register(Rv_register_id::x5, 1);
	hart.execute_slti(Rv_register_id::x1, Rv_register_id::x5, Rv_itype_imm::from_signed(-3));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);
}

/* --------------------------------------------------------
SLTIU
-------------------------------------------------------- */

TEST(execute_sltiu, SrcEqualsImm) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x5, 2);
	hart.execute_sltiu(Rv_register_id::x1, Rv_register_id::x5, Rv_itype_imm::from_unsigned(2));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);

	hart.set_register(Rv_register_id::x5, -3);
	hart.execute_sltiu(Rv_register_id::x1, Rv_register_id::x5, Rv_itype_imm::from_signed(-3));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);
}

TEST(execute_sltiu, SrcLessThanImm) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x5, 1);
	hart.execute_sltiu(Rv_register_id::x1, Rv_register_id::x5, Rv_itype_imm::from_unsigned(2));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 1);

	// This is a tricky one. -5 is less that -3. But both are interpreted as unsigned.
	// The immediate only has 12 bits, so the raw value is 0xFFD.
	// The register is 32 bits, so the raw value is 0xFFFFFFFB.
	// The result is the immediate being less than the register, so 0 is put in the destination.
	hart.set_register(Rv_register_id::x5, -5);
	hart.execute_sltiu(Rv_register_id::x1, Rv_register_id::x5, Rv_itype_imm::from_signed(-3));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);

	// 1 is not less than -3, but SLTIU uses the unsigned values
	hart.set_register(Rv_register_id::x5, 1);
	hart.execute_sltiu(Rv_register_id::x1, Rv_register_id::x5, Rv_itype_imm::from_signed(-3));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 1);
}

TEST(execute_sltiu, SrcGreaterThanImm) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x5, 3);
	hart.execute_sltiu(Rv_register_id::x1, Rv_register_id::x5, Rv_itype_imm::from_unsigned(2));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);

	hart.set_register(Rv_register_id::x5, -1);
	hart.execute_sltiu(Rv_register_id::x1, Rv_register_id::x5, Rv_itype_imm::from_signed(-3));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);

	// -5 is less than 6, but SLTIU uses the unsigned values
	hart.set_register(Rv_register_id::x5, -5);
	hart.execute_sltiu(Rv_register_id::x1, Rv_register_id::x5, Rv_itype_imm::from_unsigned(6));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);
}

TEST(execute_sltiu, ImmIsOne) {
	
	// SLTIU rd, rs1, 1 sets rd to 1 if rs1 equals zero, otherwise sets rd to 0 (assembler pseudoinstruction SEQZ rd, rs).

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	// rs1 == 0
	hart.set_register(Rv_register_id::x5, 0);
	hart.execute_sltiu(Rv_register_id::x1, Rv_register_id::x5, Rv_itype_imm::from_unsigned(1));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 1);

	// rs1 == 1
	hart.set_register(Rv_register_id::x5, 1);
	hart.execute_sltiu(Rv_register_id::x1, Rv_register_id::x5, Rv_itype_imm::from_unsigned(1));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);

	// rs1 == 2
	hart.set_register(Rv_register_id::x5, 2);
	hart.execute_sltiu(Rv_register_id::x1, Rv_register_id::x5, Rv_itype_imm::from_unsigned(1));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);
}

/* --------------------------------------------------------
SLTU
-------------------------------------------------------- */

TEST(execute_sltu, LessThan) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 2);
	hart.set_register(Rv_register_id::x3, 3);
	hart.execute_sltu(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 1);

	// -2 unsigned is greater than 3
	hart.set_register(Rv_register_id::x2, -2);
	hart.set_register(Rv_register_id::x3, 3);
	hart.execute_sltu(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);

	// -2 unsigned is less than -1 unsigned
	hart.set_register(Rv_register_id::x2, -2);
	hart.set_register(Rv_register_id::x3, -1);
	hart.execute_sltu(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 1);
}

TEST(execute_sltu, Equal) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 2);
	hart.set_register(Rv_register_id::x3, 2);
	hart.execute_sltu(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);

	hart.set_register(Rv_register_id::x2, -2);
	hart.set_register(Rv_register_id::x3, -2);
	hart.execute_sltu(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);
}

TEST(execute_sltu, GreaterThan) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 3);
	hart.set_register(Rv_register_id::x3, 2);
	hart.execute_sltu(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);

	// 2 is less than -3 unsigned
	hart.set_register(Rv_register_id::x2, 2);
	hart.set_register(Rv_register_id::x3, -3);
	hart.execute_sltu(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 1);

	// -3 unsigned is greater than -4 unsigned
	hart.set_register(Rv_register_id::x2, -3);
	hart.set_register(Rv_register_id::x3, -4);
	hart.execute_sltu(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);
}

/* --------------------------------------------------------
SRA
-------------------------------------------------------- */

TEST(execute_sra, DifferentRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0b100);
	hart.set_register(Rv_register_id::x3, 2);
	hart.execute_sra(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0b100);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 2);
}

TEST(execute_sra, SameRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 80);
	hart.execute_sra(Rv_register_id::x1, Rv_register_id::x1, Rv_register_id::x1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);
}

TEST(execute_sra, MoreThanFiveBitsSetInRs2) {

	// Shift amount is in RS2, but only the low 5 bits are used.

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0b100);
	hart.set_register(Rv_register_id::x3, 0b100001);
	hart.execute_sra(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0b10);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0b100);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0b100001);
}

TEST(execute_sra, ShiftWithSignBitSet) {

	// Arithmetic shift fills high bits with the sign bit

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0xFF000000);
	hart.set_register(Rv_register_id::x3, 8);
	hart.execute_sra(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0xFFFF0000);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0xFF000000);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 8);
}

/* --------------------------------------------------------
SRAI
-------------------------------------------------------- */

TEST(execute_srai, SignedSource) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0b1000'0000'0000'0000'0000'0000'0001'0011);
	hart.execute_srai(Rv_register_id::x1, Rv_register_id::x2, Rv_itype_imm::from_unsigned((1 << 10) | 2)); // The (1 << 10) sets the bit that indicates this is an arithmetic shift
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0b1110'0000'0000'0000'0000'0000'0000'0100);
}

TEST(execute_srai, UnsignedSource) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0b0000'0000'0000'0000'0000'0000'0001'0011);
	hart.execute_srai(Rv_register_id::x1, Rv_register_id::x2, Rv_itype_imm::from_unsigned((1 << 10) | 2)); // The (1 << 10) sets the bit that indicates this is an arithmetic shift
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0b0000'0000'0000'0000'0000'0000'0000'0100);
}

/* --------------------------------------------------------
SRL
-------------------------------------------------------- */

TEST(execute_srl, DifferentRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0b100);
	hart.set_register(Rv_register_id::x3, 2);
	hart.execute_srl(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0b100);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 2);
}

TEST(execute_srl, SameRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 80);
	hart.execute_srl(Rv_register_id::x1, Rv_register_id::x1, Rv_register_id::x1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);
}

TEST(execute_srl, MoreThanFiveBitsSetInRs2) {

	// Shift amount is in RS2, but only the low 5 bits are used.

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0b100);
	hart.set_register(Rv_register_id::x3, 0b100001);
	hart.execute_srl(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0b10);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0b100);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0b100001);
}

TEST(execute_srl, ShiftWithSignBitSet) {

	// Logical shift fills high bits with 0 regardless of sign

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0xFFFFFFFF);
	hart.set_register(Rv_register_id::x3, 8);
	hart.execute_srl(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0x00FFFFFF);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0xFFFFFFFF);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 8);
}

/* --------------------------------------------------------
SRLI
-------------------------------------------------------- */

TEST(execute_srli, SignedSource) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0b1000'0000'0000'0000'0000'0000'0001'0011);
	hart.execute_srli(Rv_register_id::x1, Rv_register_id::x2, Rv_itype_imm::from_unsigned(2));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0b0010'0000'0000'0000'0000'0000'0000'0100);
}

TEST(execute_srli, UnsignedSource) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0b0000'0000'0000'0000'0000'0000'0001'0011);
	hart.execute_srli(Rv_register_id::x1, Rv_register_id::x2, Rv_itype_imm::from_unsigned(2));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0b0000'0000'0000'0000'0000'0000'0000'0100);
}

/* --------------------------------------------------------
SUB
-------------------------------------------------------- */

TEST(execute_sub, DifferentRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 4);
	hart.set_register(Rv_register_id::x3, 14);
	hart.execute_sub(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), -10);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 14);
}

TEST(execute_sub, SameRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, -4);
	hart.execute_sub(Rv_register_id::x1, Rv_register_id::x1, Rv_register_id::x1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);
}

/* --------------------------------------------------------
SW
-------------------------------------------------------- */

TEST(execute_sw, PositiveOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 0x100);
	hart.set_register(Rv_register_id::x2, 0x40302010);
	hart.execute_sw(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(0x10));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0x100);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x40302010);
	EXPECT_EQ(memory.read_32(0x110), 0x40302010);
}

TEST(execute_sw, PositiveOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 0xFFFFFFFC);
	hart.set_register(Rv_register_id::x2, 0x40302010);
	hart.execute_sw(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(8));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0xFFFFFFFC);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x40302010);
	EXPECT_EQ(memory.read_32(4), 0x40302010);
}

TEST(execute_sw, NegativeOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 64);
	hart.set_register(Rv_register_id::x2, 0x40302010);
	hart.execute_sw(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(-32));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 64);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x40302010);
	EXPECT_EQ(memory.read_32(32), 0x40302010);
}

TEST(execute_sw, NegativeOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 4);
	hart.set_register(Rv_register_id::x2, 0x40302010);
	hart.execute_sw(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(-8));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x40302010);
	EXPECT_EQ(memory.read_32(0xFFFFFFFC), 0x40302010);
}

TEST(execute_sw, ZeroOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 6);
	hart.set_register(Rv_register_id::x2, 0x40302010);
	hart.execute_sw(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(0));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 6);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x40302010);
	EXPECT_EQ(memory.read_32(6), 0x40302010);
}

TEST(execute_sw, MisalignedAccess) {

	// Address 5 is not on a 4 or 2 byte boundary, but that is allowed

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 4);
	hart.set_register(Rv_register_id::x2, 0x40302010);
	hart.execute_sw(Rv_register_id::x1, Rv_register_id::x2, Rv_stype_imm::from_offset(1));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 4);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0x40302010);
	EXPECT_EQ(memory.read_32(5), 0x40302010);
}

/* --------------------------------------------------------
XOR
-------------------------------------------------------- */

TEST(execute_xor, DifferentRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0b1101);
	hart.set_register(Rv_register_id::x3, 0b1011);
	hart.execute_xor(Rv_register_id::x1, Rv_register_id::x2, Rv_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0b0110);
	EXPECT_EQ(hart.get_register(Rv_register_id::x2), 0b1101);
	EXPECT_EQ(hart.get_register(Rv_register_id::x3), 0b1011);
}

TEST(execute_xor, SameRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x1, 0b1110);
	hart.execute_xor(Rv_register_id::x1, Rv_register_id::x1, Rv_register_id::x1);
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0);
}

/* --------------------------------------------------------
XORI
-------------------------------------------------------- */

TEST(execute_xori, ValidInstruction) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0b01000000000000110);
	hart.execute_xori(Rv_register_id::x1, Rv_register_id::x2, Rv_itype_imm::from_signed(0b1101));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0b01000000000001011);
}

TEST(execute_xori, BitwiseNot) {

	// XORI rd, rs1, -1 performs a bitwise logical inversion of register rs1(assembler pseudoinstruction NOT rd, rs).

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv_register_id::x2, 0xFFFFFF00);
	hart.execute_xori(Rv_register_id::x1, Rv_register_id::x2, Rv_itype_imm::from_signed(-1));
	EXPECT_EQ(hart.get_register(Rv_register_id::x1), 0xFF);
}
