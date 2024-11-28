#pragma once

#include <cstdint>

class CPU;
class RAM;
class ROM;
class PPU;
class Cartridge;

class Bus
{
public:
	Bus();
	~Bus();
	uint8_t		Read (uint16_t addr);
	void		Write(uint16_t addr, uint8_t data);

	CPU* getCPU() { return cpu; }
	void DrawScreen();

	bool	getDone();
	void	setDone(bool done);

	uint8_t * getRAM();

	RAM* ram;
	Cartridge* cart;
	PPU* ppu;
private:
	bool done;
	CPU* cpu;
};

