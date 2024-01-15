#pragma once

#include <array>
#include <iostream>
#include <map>
#include <span>

#include "memory.h"
#include "rv32.h"
#include "rv32-hart.h"

namespace riscv_sim {

class Simple_memory_subsystem : public Memory
{
public:
	void write_byte(uint32_t address, uint8_t value) override;
	void write_32(uint32_t address, uint32_t value) override;
	uint8_t read_byte(uint32_t address) const override;
	uint16_t read_16(uint32_t address) const override;
	uint32_t read_32(uint32_t address) const override;

private:
	std::map<uint32_t, uint8_t> memory;
};

class Simple_system
{
public:

	Simple_system() : memory(), hart(memory) {}

	void load_program(std::span<uint32_t> instructions);

private:

	Rv32_hart hart; // Hardware thread (core)
	Simple_memory_subsystem memory;
};

}
