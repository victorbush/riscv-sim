#include <gtest/gtest.h>

#include "simple-system.h"

using namespace riscv_sim;

TEST(Simple_memory_subsystem, write_32) {

	auto system = Simple_memory_subsystem();
	system.write_32(0, 0);
	EXPECT_EQ(system.read_8(0), 0);
	EXPECT_EQ(system.read_8(1), 0);
	EXPECT_EQ(system.read_8(2), 0);
	EXPECT_EQ(system.read_8(3), 0);
	EXPECT_EQ(system.read_32(0), 0);

	system.write_32(0, 100);
	EXPECT_EQ(system.read_8(0), 100);
	EXPECT_EQ(system.read_8(1), 0);
	EXPECT_EQ(system.read_8(2), 0);
	EXPECT_EQ(system.read_8(3), 0);
	EXPECT_EQ(system.read_32(0), 100);

	system.write_32(16, 0x12345678);
	EXPECT_EQ(system.read_8(16), 0x78);
	EXPECT_EQ(system.read_8(17), 0x56);
	EXPECT_EQ(system.read_8(18), 0x34);
	EXPECT_EQ(system.read_8(19), 0x12);
	EXPECT_EQ(system.read_32(16), 0x12345678);
}
