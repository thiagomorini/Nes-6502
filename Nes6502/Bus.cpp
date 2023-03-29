#include "Bus.h"

Bus::Bus()
{
	// Connect CPU to communication with bus
	cpu.ConnectBus(this);
}

Bus::~Bus()
{
}

void Bus::cpuWrite(uint16_t addr, uint8_t data)
{
	if (addr >= 0x0000 && addr <= 0xFFFF)
		ram[addr] = data;
}

uint8_t Bus::cpuRead(uint16_t addr, bool readOnly)
{
	if (addr >= 0x0000 && addr <= 0xFFFF)
		return ram[addr];
}
