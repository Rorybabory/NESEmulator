#pragma once

#include <cstdint>



class PPU
{
public:
	PPU();
	uint8_t ReadRegister(uint8_t index);
	void WriteRegister(uint8_t index, uint8_t bits);
	void DrawScreen();
	
private:
	unsigned int scrTex;
	uint8_t registers[8];
};

