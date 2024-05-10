#pragma once

#include <cstdint>


class Bus;

class RAM
{
public:
	RAM(Bus* b);
	~RAM();

	uint8_t Read(uint16_t addr);
	void Write(uint16_t addr, uint8_t x);
	uint8_t* data;
private:
	Bus* bus;
};

