#pragma once
#include <cstdint>
#include "Cpu.h"
#include <array>

class Bus
{
public:
	Bus();
	~Bus();

public: // Devices on the bus
	Cpu cpu;

	// Fake and temporary RAM
	std::array<uint8_t, 64 * 1024> ram;

public: // CPU read and write
	void cpuWrite(uint16_t addr, uint8_t data);
	uint8_t cpuRead(uint16_t addr, bool readOnly = false);
};
