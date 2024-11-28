#pragma once

#include <cstdint>

class Bus;

class PPU
{
public:
	PPU(Bus * bus);
	uint8_t ReadRegister(uint8_t index);
	void WriteRegister(uint8_t index, uint8_t bits);
	void DrawScreen();
	
private:
	unsigned int texture;
	Bus* bus;
	unsigned int scrTex;
	uint8_t registers[8];
};

