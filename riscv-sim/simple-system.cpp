#include "simple-system.h"

#include <vector>
#include <utility>

using namespace std;

namespace riscv_sim {

/* ========================================================
Simple memory subsystem
======================================================== */

void Simple_memory_subsystem::write_8(uint32_t address, uint8_t value)
{
	memory[address] = value;
}

void Simple_memory_subsystem::write_16(uint32_t address, uint16_t value)
{
	memory[address] = 0xFF & value;
	memory[address + 1] = 0xFF & (value >> 8);
}

void Simple_memory_subsystem::write_32(uint32_t address, uint32_t value)
{
	memory[address] = 0xFF & value;
	memory[address + 1] = 0xFF & (value >> 8);
	memory[address + 2] = 0xFF & (value >> 16);
	memory[address + 3] = 0xFF & (value >> 24);
}

uint8_t Simple_memory_subsystem::read_8(uint32_t address) const
{
	if (memory.contains(address))
		return memory.at(address);

	return 0;
}

uint16_t Simple_memory_subsystem::read_16(uint32_t address) const
{
	return (read_8(address)
		| (read_8(address + 1) << 8));
}

uint32_t Simple_memory_subsystem::read_32(uint32_t address) const
{
	return (read_8(address)
		| (read_8(address + 1) << 8)
		| (read_8(address + 2) << 16)
		| (read_8(address + 3) << 24));
}

void Simple_memory_subsystem::reset()
{
	memory.clear();
}

}
