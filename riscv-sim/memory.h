#pragma once

#include <cstdint>

namespace riscv_sim {

class Memory {
public:
	virtual void write_byte(uint32_t address, uint8_t value) = 0;
	virtual void write_32(uint32_t address, uint32_t value) = 0;
	virtual uint8_t read_byte(uint32_t address) const = 0;
	virtual uint16_t read_16(uint32_t address) const = 0;
	virtual uint32_t read_32(uint32_t address) const = 0;
};

}
