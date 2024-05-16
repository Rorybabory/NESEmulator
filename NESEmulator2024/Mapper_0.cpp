#include "Mapper_0.h"

#include "Cartridge.h"

uint8_t Mapper_0::Read(uint16_t address) {
	if (address >= 0x8000) {
		return cart->prgROM[(uint32_t)address - 0x8000];
	}
	return 0;
}
void Mapper_0::Write(uint16_t address, uint8_t data) {	
	if (address >= 0x8000) {
		cart->prgROM[(uint32_t)address - 0x8000] = data;
	}
}