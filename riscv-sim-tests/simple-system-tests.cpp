#include <gtest/gtest.h>

#include "simple-system.h"

using namespace riscv_sim;

//TEST(Simple_system, execute_rv32i_addi) {
//	auto system = Simple_system();
//	auto pc = system.registers[Rv32_register_id::pc];
//	
//	system.execute_rv32i_addi()
//}

TEST(Simple_memory_subsystem, write_32) {

	auto system = Simple_memory_subsystem();
	system.write_32(0, 0);
	EXPECT_EQ(system.read_byte(0), 0);
	EXPECT_EQ(system.read_byte(1), 0);
	EXPECT_EQ(system.read_byte(2), 0);
	EXPECT_EQ(system.read_byte(3), 0);
	EXPECT_EQ(system.read_32(0), 0);

	system.write_32(0, 100);
	EXPECT_EQ(system.read_byte(0), 100);
	EXPECT_EQ(system.read_byte(1), 0);
	EXPECT_EQ(system.read_byte(2), 0);
	EXPECT_EQ(system.read_byte(3), 0);
	EXPECT_EQ(system.read_32(0), 100);

	system.write_32(16, 0x12345678);
	EXPECT_EQ(system.read_byte(16), 0x78);
	EXPECT_EQ(system.read_byte(17), 0x56);
	EXPECT_EQ(system.read_byte(18), 0x34);
	EXPECT_EQ(system.read_byte(19), 0x12);
	EXPECT_EQ(system.read_32(16), 0x12345678);
}
