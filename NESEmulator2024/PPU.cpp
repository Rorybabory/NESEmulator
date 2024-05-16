#include "PPU.h"


uint8_t PPU::ReadRegister(uint8_t index) {
	return registers[index];
}
void PPU::WriteRegister(uint8_t index, uint8_t bits) {
	registers[index] = bits;
}

void PPU::DrawScreen() {
	
}
PPU::PPU() {
	for (int i = 0; i < 8; ++i) {
		registers[i] = 0;
	}
	scrTex = 0;
}