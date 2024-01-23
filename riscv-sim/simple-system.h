#pragma once

#include <map>

#include "memory.h"

namespace riscv_sim {

class Simple_memory_subsystem : public Memory
{
public:
	void write_8(uint32_t address, uint8_t value) override;
	void write_16(uint32_t address, uint16_t value) override;
	void write_32(uint32_t address, uint32_t value) override;
	uint8_t read_8(uint32_t address) const override;
	uint16_t read_16(uint32_t address) const override;
	uint32_t read_32(uint32_t address) const override;
	
	void reset();

private:
	std::map<uint32_t, uint8_t> memory;
};

}
