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
	void ClearScreen();
	void DrawTile(unsigned int index, uint8_t x, uint8_t y, uint8_t colors[4]);
private:
	unsigned int texture;
	Bus* bus;
	unsigned int scrTex;
	uint8_t registers[8];
	uint8_t* screen_data;
	uint8_t* texture_data;

};

