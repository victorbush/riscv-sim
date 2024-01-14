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

	auto instruction = Rv32_encoder::encode_add(Rv32_register_id::x2, Rv32_register_id::x3, Rv32_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.set_register(Rv32_register_id::x4, 10);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 14);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x4), 10);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, ADDI) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_addi(Rv32_register_id::x2, Rv32_register_id::x3, 5);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 9);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, AND) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_and(Rv32_register_id::x2, Rv32_register_id::x3, Rv32_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x3, 0b1010);
	hart.set_register(Rv32_register_id::x4, 0b1001);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0b1000);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 0b1010);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x4), 0b1001);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, ANDI) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_andi(Rv32_register_id::x2, Rv32_register_id::x3, 0b010);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x3, 0b011);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0b010);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 0b011);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, AUIPC) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_auipc(Rv32_register_id::x2, 0b1111);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), (0x500 + 0b1111'0000'0000'0000));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, BEQ) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_beq(Rv32_register_id::x2, Rv32_register_id::x3, 0x10);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x2, 1);
	hart.set_register(Rv32_register_id::x3, 1);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x510);
}

TEST(execute_next, BGE) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_bge(Rv32_register_id::x2, Rv32_register_id::x3, 0x10);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x2, 3);
	hart.set_register(Rv32_register_id::x3, 2);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 2);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x510);
}

TEST(execute_next, BGEU) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_bgeu(Rv32_register_id::x2, Rv32_register_id::x3, 0x10);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x2, -2);
	hart.set_register(Rv32_register_id::x3, 3);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), -2);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x510);
}

TEST(execute_next, BLT) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_blt(Rv32_register_id::x2, Rv32_register_id::x3, 0x10);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x2, 2);
	hart.set_register(Rv32_register_id::x3, 3);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 2);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x510);
}

TEST(execute_next, BLTU) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_bltu(Rv32_register_id::x2, Rv32_register_id::x3, 0x10);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x2, 2);
	hart.set_register(Rv32_register_id::x3, -3);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 2);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), -3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x510);
}

TEST(execute_next, BNE) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_bne(Rv32_register_id::x2, Rv32_register_id::x3, 0x10);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x2, 1);
	hart.set_register(Rv32_register_id::x3, 2);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 2);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x510);
}

TEST(execute_next, LB) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_lb(Rv32_register_id::x2, Rv32_register_id::x3, 4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x2, 1);
	hart.set_register(Rv32_register_id::x3, 2);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 2);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x510);
}

TEST(execute_next, LUI) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_lui(Rv32_register_id::x2, 0b1111);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0b1111'0000'0000'0000);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, NOP) {

	// NOP isn't an actual instruction, it's encoded as ADDI x0, x0, 0.

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_addi(Rv32_register_id::x0, Rv32_register_id::x0, 0);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x0), 0);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, OR) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_or(Rv32_register_id::x2, Rv32_register_id::x3, Rv32_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x3, 0b1010);
	hart.set_register(Rv32_register_id::x4, 0b1001);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0b1011);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 0b1010);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x4), 0b1001);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, ORI) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_ori(Rv32_register_id::x2, Rv32_register_id::x3, 0b010);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x3, 0b011);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0b011);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 0b011);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, SLL) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_sll(Rv32_register_id::x2, Rv32_register_id::x3, Rv32_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x3, 0b1101);
	hart.set_register(Rv32_register_id::x4, 3);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0b1101000);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 0b1101);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x4), 3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, SLLI) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_slli(Rv32_register_id::x2, Rv32_register_id::x3, 2);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x3, 0b11);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0b1100);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 0b0011);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, SLT) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_slt(Rv32_register_id::x2, Rv32_register_id::x3, Rv32_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x3, -1);
	hart.set_register(Rv32_register_id::x4, 1);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), -1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x4), 1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, SLTI) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_slti(Rv32_register_id::x2, Rv32_register_id::x3, 5);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x3, 3);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, SLTIU) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_sltiu(Rv32_register_id::x2, Rv32_register_id::x3, 5);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x3, -3);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), -3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, SLTU) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_sltu(Rv32_register_id::x2, Rv32_register_id::x3, Rv32_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.set_register(Rv32_register_id::x4, 10);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x4), 10);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, SRA) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_sra(Rv32_register_id::x2, Rv32_register_id::x3, Rv32_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x3, 0xFF000000);
	hart.set_register(Rv32_register_id::x4, 8);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0xFFFF0000);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 0xFF000000);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x4), 8);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, SRAI) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_srai(Rv32_register_id::x2, Rv32_register_id::x3, 2);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x3, -1);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), -1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), -1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, SRL) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_srl(Rv32_register_id::x2, Rv32_register_id::x3, Rv32_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x3, 0xFF000000);
	hart.set_register(Rv32_register_id::x4, 8);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0x00FF0000);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 0xFF000000);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x4), 8);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, SRLI) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_srli(Rv32_register_id::x2, Rv32_register_id::x3, 4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x3, 0xFFFFFFFF);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0x0FFFFFFF);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), -1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, SUB) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_sub(Rv32_register_id::x2, Rv32_register_id::x3, Rv32_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.set_register(Rv32_register_id::x4, 10);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), -6);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x4), 10);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, XOR) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_xor(Rv32_register_id::x2, Rv32_register_id::x3, Rv32_register_id::x4);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x3, 0b1010);
	hart.set_register(Rv32_register_id::x4, 0b1001);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0b0011);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 0b1010);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x4), 0b1001);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

TEST(execute_next, XORI) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	auto instruction = Rv32_encoder::encode_xori(Rv32_register_id::x2, Rv32_register_id::x3, 0b010);
	memory.write_32(0x500, instruction);

	hart.set_register(Rv32_register_id::pc, 0x500);
	hart.set_register(Rv32_register_id::x3, 0b110);
	hart.execute_next();

	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0b100);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 0b110);
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 0x504);
}

/* --------------------------------------------------------
ADD
-------------------------------------------------------- */

TEST(execute_add, DifferentRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 14);
	hart.execute_add(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 18);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 14);
}

TEST(execute_add, SameRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x1, 4);
	hart.execute_add(Rv32_register_id::x1, Rv32_register_id::x1, Rv32_register_id::x1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 8);
}

/* --------------------------------------------------------
ADDI
-------------------------------------------------------- */

TEST(execute_addi, AddZeroToX0) {

	// Add zero and store in zero register
	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.execute_addi(Rv32_register_id::x0, Rv32_register_id::x1, 0);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x0), 0);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);
}

TEST(execute_addi, AddNonZeroToX0) {

	// Add non-zero and store in zero register (zero register can't be changed)
	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.execute_addi(Rv32_register_id::x0, Rv32_register_id::x1, 21);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x0), 0);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);
}

TEST(execute_addi, AddWithSameDestAndSrc) {

	// Use same register for rd and rs1
	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.execute_addi(Rv32_register_id::x1, Rv32_register_id::x1, 21);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 21);

	// Repeat with a non-zero value in rs1
	hart.set_register(Rv32_register_id::x1, 10);
	hart.execute_addi(Rv32_register_id::x1, Rv32_register_id::x1, 21);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 31);
}

TEST(execute_addi, AddWithDiffDestAndSrc) {

	// Use different register for rd and rs1
	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.execute_addi(Rv32_register_id::x1, Rv32_register_id::x2, 21);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 21);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0);

	// Repeat with a non-zero value in rs1
	hart.set_register(Rv32_register_id::x2, 10);
	hart.execute_addi(Rv32_register_id::x1, Rv32_register_id::x2, 21);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 31);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 10);
}

TEST(execute_addi, OverflowIgnored) {

	// Arithmetic overflow is ignored and the result is simply the low XLEN bits of the result.
	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x5, 0xFFFFFFFF);
	hart.execute_addi(Rv32_register_id::x5, Rv32_register_id::x5, 1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x5), 0);
}

TEST(execute_addi, NOP) {

	// NOP is encoded as ADDI x0, x0, 0
	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.execute_addi(Rv32_register_id::x0, Rv32_register_id::x0, 0);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x0), 0);
}

/* --------------------------------------------------------
AND
-------------------------------------------------------- */

TEST(execute_and, DifferentRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0b1101);
	hart.set_register(Rv32_register_id::x3, 0b1011);
	hart.execute_and(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0b1001);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0b1101);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 0b1011);
}

TEST(execute_and, SameRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x1, 0b1110);
	hart.execute_and(Rv32_register_id::x1, Rv32_register_id::x1, Rv32_register_id::x1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0b1110);
}

/* --------------------------------------------------------
ANDI
-------------------------------------------------------- */

TEST(execute_andi, NoMatchingBits) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0b01000000000000110);
	hart.execute_andi(Rv32_register_id::x1, Rv32_register_id::x2, 0b1001);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);
}

TEST(execute_andi, MatchingBit) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0b01000000000000110);
	hart.execute_andi(Rv32_register_id::x1, Rv32_register_id::x2, 0b0100);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0b0100);
}

TEST(execute_andi, ImmediateGreaterThan12Bits) {

	// Immediate value gets truncated to only 12 bits
	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0b01000000000000110);
	hart.execute_andi(Rv32_register_id::x1, Rv32_register_id::x2, 0b01000000000000110);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0b0110);
}

/* --------------------------------------------------------
AUIPC
-------------------------------------------------------- */

TEST(execute_auipc, ValidInstruction) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 0x200);

	// Only 20-bit immediate, ensure top bits are ignored
	hart.execute_auipc(Rv32_register_id::x1, 0b1111'0101'1111'0101'1111'0101);

	// Ensure 20-bit immediate is placed in high 20 bits
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0x200 + 0b0101'1111'0101'1111'0101'0000'0000'0000);
}

/* --------------------------------------------------------
BEQ
-------------------------------------------------------- */

TEST(execute_beq, BranchTakenWithPositiveOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.execute_beq(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);
}

TEST(execute_beq, BranchTakenWithNegativeOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 464);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.execute_beq(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 400);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);
}

TEST(execute_beq, BranchNotTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 464);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 7);
	hart.execute_beq(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 468); // PC just gets advanced to next instruction
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 7);
}

TEST(execute_beq, AddressMisalignedAndBranchTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 4);

	EXPECT_THROW_EX(hart.execute_beq(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(2)), "instruction-address-misaligned");
}

TEST(execute_beq, AddressMisalignedButBranchNotTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 7);
	hart.execute_beq(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(2));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 404); // PC just gets advanced to next instruction
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 7);
}

/* --------------------------------------------------------
BGE
-------------------------------------------------------- */

TEST(execute_bge, BranchTakenBecauseGreaterThan) {

	// rs1 > rs2, branch taken

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 5);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.execute_bge(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 5);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);

	// Ensure signed comparison

	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 5);
	hart.set_register(Rv32_register_id::x3, -4);
	hart.execute_bge(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 5);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), -4);
}

TEST(execute_bge, BranchTakenBecauseEqualTo) {

	// rs1 == rs2, branch taken

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.execute_bge(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);
}

TEST(execute_bge, BranchNotTaken) {

	// rs1 < rs2, so PC gets advanced to next instruction

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 464);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 5);
	hart.execute_bge(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 468);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 5);

	// Ensure signed comparison

	hart.set_register(Rv32_register_id::pc, 464);
	hart.set_register(Rv32_register_id::x2, -5);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.execute_bge(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 468);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), -5);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);
}

TEST(execute_bge, AddressMisalignedAndBranchTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 3);

	EXPECT_THROW_EX(hart.execute_bge(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(2)), "instruction-address-misaligned");
}

TEST(execute_bge, AddressMisalignedButBranchNotTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 5);
	hart.execute_bge(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(2));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 404);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 5);
}

/* --------------------------------------------------------
BGEU
-------------------------------------------------------- */

TEST(execute_bgeu, BranchTakenBecauseGreaterThan) {

	// rs1 > rs2, branch taken

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 5);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.execute_bgeu(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 5);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);

	// Ensure UNSIGNED comparison
	// -5 is less than 4, but -5 will interpreted as unsigned

	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, -5);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.execute_bgeu(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), -5);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);
}

TEST(execute_bgeu, BranchTakenBecauseEqualTo) {

	// rs1 == rs2, branch taken

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.execute_bgeu(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);
}

TEST(execute_bgeu, BranchNotTaken) {

	// rs1 < rs2, so PC gets advanced to next instruction

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 464);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 5);
	hart.execute_bgeu(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 468);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 5);

	// Ensure UNSIGNED comparison
	// 4 is greater than -5, but -5 will interpreted as unsigned

	hart.set_register(Rv32_register_id::pc, 464);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, -5);
	hart.execute_bgeu(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 468);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), -5);
}

TEST(execute_bgeu, AddressMisalignedAndBranchTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 3);

	EXPECT_THROW_EX(hart.execute_bgeu(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(2)), "instruction-address-misaligned");
}

TEST(execute_bgeu, AddressMisalignedButBranchNotTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 5);
	hart.execute_bgeu(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(2));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 404);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 5);
}

/* --------------------------------------------------------
BLT
-------------------------------------------------------- */

TEST(execute_blt, BranchTaken) {

	// rs1 < rs2, branch taken

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 5);
	hart.execute_blt(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 5);

	// Ensure signed comparison

	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, -4);
	hart.set_register(Rv32_register_id::x3, 5);
	hart.execute_blt(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), -4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 5);
}

TEST(execute_blt, BranchNotTakenBecauseEqualTo) {

	// rs1 == rs2, so PC gets advanced to next instruction

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.execute_blt(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 404);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);
}

TEST(execute_blt, BranchNotTakenBecauseGreaterThan) {

	// rs1 > rs2, so PC gets advanced to next instruction

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 464);
	hart.set_register(Rv32_register_id::x2, 5);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.execute_blt(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 468);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 5);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);

	// Ensure signed comparison

	hart.set_register(Rv32_register_id::pc, 464);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, -5);
	hart.execute_blt(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 468);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), -5);
}

TEST(execute_blt, AddressMisalignedAndBranchTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 3);
	hart.set_register(Rv32_register_id::x3, 4);

	EXPECT_THROW_EX(hart.execute_blt(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(2)), "instruction-address-misaligned");
}

TEST(execute_blt, AddressMisalignedButBranchNotTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 3);
	hart.execute_blt(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(2));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 404);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 3);
}

/* --------------------------------------------------------
BLTU
-------------------------------------------------------- */

TEST(execute_bltu, BranchTaken) {

	// rs1 < rs2, branch taken

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 5);
	hart.execute_bltu(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 5);

	// Ensure UNSIGNED comparison
	// 5 is greater than -4, but -4 is interpreted as unsigned

	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 5);
	hart.set_register(Rv32_register_id::x3, -4);
	hart.execute_bltu(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 5);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), -4);
}

TEST(execute_bltu, BranchNotTakenBecauseEqualTo) {

	// rs1 == rs2, so PC gets advanced to next instruction

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.execute_bltu(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 404);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);
}

TEST(execute_bltu, BranchNotTakenBecauseGreaterThan) {

	// rs1 > rs2, so PC gets advanced to next instruction

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 464);
	hart.set_register(Rv32_register_id::x2, 5);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.execute_bltu(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 468);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 5);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);

	// Ensure UNSIGNED comparison
	// -5 is less than 4, but -5 is interpreted as unsigned

	hart.set_register(Rv32_register_id::pc, 464);
	hart.set_register(Rv32_register_id::x2, -5);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.execute_bltu(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 468);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), -5);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);
}

TEST(execute_bltu, AddressMisalignedAndBranchTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 3);
	hart.set_register(Rv32_register_id::x3, 4);

	EXPECT_THROW_EX(hart.execute_bltu(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(2)), "instruction-address-misaligned");
}

TEST(execute_bltu, AddressMisalignedButBranchNotTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 3);
	hart.execute_bltu(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(2));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 404);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 3);
}

/* --------------------------------------------------------
BNE
-------------------------------------------------------- */

TEST(execute_bne, BranchTakenWithPositiveOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 5);
	hart.execute_bne(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 464);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 5);
}

TEST(execute_bne, BranchTakenWithNegativeOffset) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 464);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 5);
	hart.execute_bne(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 400);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 5);
}

TEST(execute_bne, BranchNotTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 464);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.execute_bne(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(-64));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 468); // PC just gets advanced to next instruction
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);
}

TEST(execute_bne, AddressMisalignedAndBranchTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 7);

	EXPECT_THROW_EX(hart.execute_bne(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(2)), "instruction-address-misaligned");
}

TEST(execute_bne, AddressMisalignedButBranchNotTaken) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::pc, 400);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.execute_bne(Rv32_register_id::x2, Rv32_register_id::x3, Rv_btype_imm::from_offset(2));
	EXPECT_EQ(hart.get_register(Rv32_register_id::pc), 404); // PC just gets advanced to next instruction
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);
}

/* --------------------------------------------------------
LB
-------------------------------------------------------- */

TEST(execute_lb, PositiveOffset) {

	auto memory = Simple_memory_subsystem();
	memory.write_byte(96, 1);
	memory.write_byte(97, 2);
	memory.write_byte(98, 3);
	memory.write_byte(99, 4);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x3, 64);
	hart.execute_lb(Rv32_register_id::x2, Rv32_register_id::x3, Rv_itype_imm(32));
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 64);
}

TEST(execute_lb, PositiveOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	memory.write_byte(4, 10);
	memory.write_byte(5, 20);
	memory.write_byte(6, 30);
	memory.write_byte(7, 40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x3, 0xFFFFFFFC);
	hart.execute_lb(Rv32_register_id::x2, Rv32_register_id::x3, Rv_itype_imm(8));
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 10);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 0xFFFFFFFC);
}

TEST(execute_lb, NegativeOffset) {

	auto memory = Simple_memory_subsystem();
	memory.write_byte(32, 10);
	memory.write_byte(33, 20);
	memory.write_byte(34, 30);
	memory.write_byte(35, 40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x3, 64);
	hart.execute_lb(Rv32_register_id::x2, Rv32_register_id::x3, Rv_itype_imm(-32));
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 10);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 64);
}

TEST(execute_lb, NegativeOffsetWrapAround) {

	auto memory = Simple_memory_subsystem();
	memory.write_byte(0xFFFFFFFC, 10);
	memory.write_byte(0xFFFFFFFD, 20);
	memory.write_byte(0xFFFFFFFE, 30);
	memory.write_byte(0xFFFFFFFF, 40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.execute_lb(Rv32_register_id::x2, Rv32_register_id::x3, Rv_itype_imm(-8));
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 10);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);
}

TEST(execute_lb, ZeroOffset) {

	auto memory = Simple_memory_subsystem();
	memory.write_byte(4, 10);
	memory.write_byte(5, 20);
	memory.write_byte(6, 30);
	memory.write_byte(7, 40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x3, 6);
	hart.execute_lb(Rv32_register_id::x2, Rv32_register_id::x3, Rv_itype_imm(0));
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 30);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 6);
}

TEST(execute_lb, MisalignedAccess) {

	// Address 5 is not on a 4 or 2 byte boundary, but that is allowed

	auto memory = Simple_memory_subsystem();
	memory.write_byte(4, 10);
	memory.write_byte(5, 20);
	memory.write_byte(6, 30);
	memory.write_byte(7, 40);

	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.execute_lb(Rv32_register_id::x2, Rv32_register_id::x3, Rv_itype_imm(1));
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 20);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);
}

/* --------------------------------------------------------
LUI
-------------------------------------------------------- */

TEST(execute_lui, ValidInstruction) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	// Only 20-bit immediate, ensure top bits are ignored
	hart.execute_lui(Rv32_register_id::x1, 0b1111'0101'1111'0101'1111'0101);
	
	// Ensure 20-bit immediate is placed in high 20 bits
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0b0101'1111'0101'1111'0101'0000'0000'0000);
}

/* --------------------------------------------------------
OR
-------------------------------------------------------- */

TEST(execute_or, DifferentRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0b1101);
	hart.set_register(Rv32_register_id::x3, 0b1011);
	hart.execute_or(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0b1111);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0b1101);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 0b1011);
}

TEST(execute_or, SameRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x1, 0b1110);
	hart.execute_or(Rv32_register_id::x1, Rv32_register_id::x1, Rv32_register_id::x1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0b1110);
}

/* --------------------------------------------------------
ORI
-------------------------------------------------------- */

TEST(execute_ori, ValidInstruction) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0b01000000000000110);
	hart.execute_ori(Rv32_register_id::x1, Rv32_register_id::x2, 0b1001);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0b01000000000001111);
}

/* --------------------------------------------------------
SLL
-------------------------------------------------------- */

TEST(execute_sll, DifferentRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 1);
	hart.set_register(Rv32_register_id::x3, 4);
	hart.execute_sll(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0b10000);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 4);
}

TEST(execute_sll, SameRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x1, 0b10);
	hart.execute_sll(Rv32_register_id::x1, Rv32_register_id::x1, Rv32_register_id::x1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0b1000);
}

TEST(execute_sll, MoreThanFiveBitsSetInRs2) {

	// Shift amount is in RS2, but only the low 5 bits are used.

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 1);
	hart.set_register(Rv32_register_id::x3, 0b100001);
	hart.execute_sll(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 2);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 0b100001);
}

TEST(execute_sll, ShiftOutOfHighBit) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0xFFFFFFFF);
	hart.set_register(Rv32_register_id::x3, 8);
	hart.execute_sll(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0xFFFFFF00);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0xFFFFFFFF);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 8);
}

/* --------------------------------------------------------
SLLI
-------------------------------------------------------- */

TEST(execute_slli, ValidInstruction) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0b0100'0000'0000'0000'0000'0000'0000'0011);
	hart.execute_slli(Rv32_register_id::x1, Rv32_register_id::x2, 2);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0b1100);
}

/* --------------------------------------------------------
SLT
-------------------------------------------------------- */

TEST(execute_slt, LessThan) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 2);
	hart.set_register(Rv32_register_id::x3, 3);
	hart.execute_slt(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 1);

	hart.set_register(Rv32_register_id::x2, -2);
	hart.set_register(Rv32_register_id::x3, 3);
	hart.execute_slt(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 1);

	hart.set_register(Rv32_register_id::x2, -2);
	hart.set_register(Rv32_register_id::x3, -1);
	hart.execute_slt(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 1);
}

TEST(execute_slt, Equal) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 2);
	hart.set_register(Rv32_register_id::x3, 2);
	hart.execute_slt(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);

	hart.set_register(Rv32_register_id::x2, -2);
	hart.set_register(Rv32_register_id::x3, -2);
	hart.execute_slt(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);
}

TEST(execute_slt, GreaterThan) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 3);
	hart.set_register(Rv32_register_id::x3, 2);
	hart.execute_slt(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);

	hart.set_register(Rv32_register_id::x2, 2);
	hart.set_register(Rv32_register_id::x3, -3);
	hart.execute_slt(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);

	hart.set_register(Rv32_register_id::x2, -3);
	hart.set_register(Rv32_register_id::x3, -4);
	hart.execute_slt(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);
}

/* --------------------------------------------------------
SLTI
-------------------------------------------------------- */

TEST(execute_slti, SrcEqualsImm) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x5, 2);
	hart.execute_slti(Rv32_register_id::x1, Rv32_register_id::x5, 2);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);

	hart.set_register(Rv32_register_id::x5, -3);
	hart.execute_slti(Rv32_register_id::x1, Rv32_register_id::x5, -3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);
}

TEST(execute_slti, SrcLessThanImm) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x5, 1);
	hart.execute_slti(Rv32_register_id::x1, Rv32_register_id::x5, 2);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 1);

	hart.set_register(Rv32_register_id::x5, -5);
	hart.execute_slti(Rv32_register_id::x1, Rv32_register_id::x5, -3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 1);

	hart.set_register(Rv32_register_id::x5, -5);
	hart.execute_slti(Rv32_register_id::x1, Rv32_register_id::x5, 6);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 1);
}

TEST(execute_slti, SrcGreaterThanImm) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x5, 3);
	hart.execute_slti(Rv32_register_id::x1, Rv32_register_id::x5, 2);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);

	hart.set_register(Rv32_register_id::x5, -1);
	hart.execute_slti(Rv32_register_id::x1, Rv32_register_id::x5, -3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);

	hart.set_register(Rv32_register_id::x5, 1);
	hart.execute_slti(Rv32_register_id::x1, Rv32_register_id::x5, -3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);
}

/* --------------------------------------------------------
SLTIU
-------------------------------------------------------- */

TEST(execute_sltiu, SrcEqualsImm) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x5, 2);
	hart.execute_sltiu(Rv32_register_id::x1, Rv32_register_id::x5, 2);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);

	hart.set_register(Rv32_register_id::x5, -3);
	hart.execute_sltiu(Rv32_register_id::x1, Rv32_register_id::x5, -3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);
}

TEST(execute_sltiu, SrcLessThanImm) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x5, 1);
	hart.execute_sltiu(Rv32_register_id::x1, Rv32_register_id::x5, 2);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 1);

	hart.set_register(Rv32_register_id::x5, -5);
	hart.execute_sltiu(Rv32_register_id::x1, Rv32_register_id::x5, -3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 1);

	// 1 is not less than -3, but SLTIU uses the unsigned values
	hart.set_register(Rv32_register_id::x5, 1);
	hart.execute_sltiu(Rv32_register_id::x1, Rv32_register_id::x5, -3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 1);
}

TEST(execute_sltiu, SrcGreaterThanImm) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x5, 3);
	hart.execute_sltiu(Rv32_register_id::x1, Rv32_register_id::x5, 2);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);

	hart.set_register(Rv32_register_id::x5, -1);
	hart.execute_sltiu(Rv32_register_id::x1, Rv32_register_id::x5, -3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);

	// -5 is less than 6, but SLTIU uses the unsigned values
	hart.set_register(Rv32_register_id::x5, -5);
	hart.execute_sltiu(Rv32_register_id::x1, Rv32_register_id::x5, 6);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);
}

TEST(execute_sltiu, ImmIsOne) {
	
	// SLTIU rd, rs1, 1 sets rd to 1 if rs1 equals zero, otherwise sets rd to 0 (assembler pseudoinstruction SEQZ rd, rs).

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);

	// rs1 == 0
	hart.set_register(Rv32_register_id::x5, 0);
	hart.execute_sltiu(Rv32_register_id::x1, Rv32_register_id::x5, 1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 1);

	// rs1 == 1
	hart.set_register(Rv32_register_id::x5, 1);
	hart.execute_sltiu(Rv32_register_id::x1, Rv32_register_id::x5, 1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);

	// rs1 == 2
	hart.set_register(Rv32_register_id::x5, 2);
	hart.execute_sltiu(Rv32_register_id::x1, Rv32_register_id::x5, 1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);
}

/* --------------------------------------------------------
SLTU
-------------------------------------------------------- */

TEST(execute_sltu, LessThan) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 2);
	hart.set_register(Rv32_register_id::x3, 3);
	hart.execute_sltu(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 1);

	// -2 unsigned is greater than 3
	hart.set_register(Rv32_register_id::x2, -2);
	hart.set_register(Rv32_register_id::x3, 3);
	hart.execute_sltu(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);

	// -2 unsigned is less than -1 unsigned
	hart.set_register(Rv32_register_id::x2, -2);
	hart.set_register(Rv32_register_id::x3, -1);
	hart.execute_sltu(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 1);
}

TEST(execute_sltu, Equal) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 2);
	hart.set_register(Rv32_register_id::x3, 2);
	hart.execute_sltu(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);

	hart.set_register(Rv32_register_id::x2, -2);
	hart.set_register(Rv32_register_id::x3, -2);
	hart.execute_sltu(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);
}

TEST(execute_sltu, GreaterThan) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 3);
	hart.set_register(Rv32_register_id::x3, 2);
	hart.execute_sltu(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);

	// 2 is less than -3 unsigned
	hart.set_register(Rv32_register_id::x2, 2);
	hart.set_register(Rv32_register_id::x3, -3);
	hart.execute_sltu(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 1);

	// -3 unsigned is greater than -4 unsigned
	hart.set_register(Rv32_register_id::x2, -3);
	hart.set_register(Rv32_register_id::x3, -4);
	hart.execute_sltu(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);
}

/* --------------------------------------------------------
SRA
-------------------------------------------------------- */

TEST(execute_sra, DifferentRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0b100);
	hart.set_register(Rv32_register_id::x3, 2);
	hart.execute_sra(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0b100);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 2);
}

TEST(execute_sra, SameRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x1, 80);
	hart.execute_sra(Rv32_register_id::x1, Rv32_register_id::x1, Rv32_register_id::x1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);
}

TEST(execute_sra, MoreThanFiveBitsSetInRs2) {

	// Shift amount is in RS2, but only the low 5 bits are used.

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0b100);
	hart.set_register(Rv32_register_id::x3, 0b100001);
	hart.execute_sra(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0b10);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0b100);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 0b100001);
}

TEST(execute_sra, ShiftWithSignBitSet) {

	// Arithmetic shift fills high bits with the sign bit

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0xFF000000);
	hart.set_register(Rv32_register_id::x3, 8);
	hart.execute_sra(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0xFFFF0000);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0xFF000000);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 8);
}

/* --------------------------------------------------------
SRAI
-------------------------------------------------------- */

TEST(execute_srai, SignedSource) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0b1000'0000'0000'0000'0000'0000'0001'0011);
	hart.execute_srai(Rv32_register_id::x1, Rv32_register_id::x2, (1 << 10) | 2); // The (1 << 10) sets the bit that indicates this is an arithmetic shift
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0b1110'0000'0000'0000'0000'0000'0000'0100);
}

TEST(execute_srai, UnsignedSource) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0b0000'0000'0000'0000'0000'0000'0001'0011);
	hart.execute_srai(Rv32_register_id::x1, Rv32_register_id::x2, (1 << 10) | 2); // The (1 << 10) sets the bit that indicates this is an arithmetic shift
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0b0000'0000'0000'0000'0000'0000'0000'0100);
}

/* --------------------------------------------------------
SRL
-------------------------------------------------------- */

TEST(execute_srl, DifferentRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0b100);
	hart.set_register(Rv32_register_id::x3, 2);
	hart.execute_srl(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0b100);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 2);
}

TEST(execute_srl, SameRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x1, 80);
	hart.execute_srl(Rv32_register_id::x1, Rv32_register_id::x1, Rv32_register_id::x1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);
}

TEST(execute_srl, MoreThanFiveBitsSetInRs2) {

	// Shift amount is in RS2, but only the low 5 bits are used.

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0b100);
	hart.set_register(Rv32_register_id::x3, 0b100001);
	hart.execute_srl(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0b10);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0b100);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 0b100001);
}

TEST(execute_srl, ShiftWithSignBitSet) {

	// Logical shift fills high bits with 0 regardless of sign

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0xFFFFFFFF);
	hart.set_register(Rv32_register_id::x3, 8);
	hart.execute_srl(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0x00FFFFFF);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0xFFFFFFFF);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 8);
}

/* --------------------------------------------------------
SRLI
-------------------------------------------------------- */

TEST(execute_srli, SignedSource) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0b1000'0000'0000'0000'0000'0000'0001'0011);
	hart.execute_srli(Rv32_register_id::x1, Rv32_register_id::x2, 2);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0b0010'0000'0000'0000'0000'0000'0000'0100);
}

TEST(execute_srli, UnsignedSource) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0b0000'0000'0000'0000'0000'0000'0001'0011);
	hart.execute_srli(Rv32_register_id::x1, Rv32_register_id::x2, 2);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0b0000'0000'0000'0000'0000'0000'0000'0100);
}

/* --------------------------------------------------------
SUB
-------------------------------------------------------- */

TEST(execute_sub, DifferentRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 4);
	hart.set_register(Rv32_register_id::x3, 14);
	hart.execute_sub(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), -10);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 4);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 14);
}

TEST(execute_sub, SameRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x1, -4);
	hart.execute_sub(Rv32_register_id::x1, Rv32_register_id::x1, Rv32_register_id::x1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);
}

/* --------------------------------------------------------
XOR
-------------------------------------------------------- */

TEST(execute_xor, DifferentRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0b1101);
	hart.set_register(Rv32_register_id::x3, 0b1011);
	hart.execute_xor(Rv32_register_id::x1, Rv32_register_id::x2, Rv32_register_id::x3);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0b0110);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x2), 0b1101);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x3), 0b1011);
}

TEST(execute_xor, SameRegisters) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x1, 0b1110);
	hart.execute_xor(Rv32_register_id::x1, Rv32_register_id::x1, Rv32_register_id::x1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0);
}

/* --------------------------------------------------------
XORI
-------------------------------------------------------- */

TEST(execute_xori, ValidInstruction) {

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0b01000000000000110);
	hart.execute_xori(Rv32_register_id::x1, Rv32_register_id::x2, 0b1101);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0b01000000000001011);
}

TEST(execute_xori, BitwiseNot) {

	// XORI rd, rs1, -1 performs a bitwise logical inversion of register rs1(assembler pseudoinstruction NOT rd, rs).

	auto memory = Simple_memory_subsystem();
	auto hart = Rv32_hart(memory);
	hart.set_register(Rv32_register_id::x2, 0xFFFFFF00);
	hart.execute_xori(Rv32_register_id::x1, Rv32_register_id::x2, -1);
	EXPECT_EQ(hart.get_register(Rv32_register_id::x1), 0xFF);
}
