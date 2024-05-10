#pragma once

#include <cstdint>

class CPU;
class RAM;
class ROM;

class Bus
{
public:
	Bus();
	~Bus();
	uint8_t		Read (uint16_t addr);
	void		Write(uint16_t addr, uint8_t data);

	CPU* getCPU() { return cpu; }

	bool	getDone();
	void	setDone(bool done);

	uint8_t * getRAM();

private:
	bool done;
	CPU* cpu;
	RAM* ram;
	ROM* rom;
};

