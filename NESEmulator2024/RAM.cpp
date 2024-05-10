#include "RAM.h"

RAM::RAM(Bus* b) { 
	this->bus = b;
	data = new uint8_t[0x800];
	for (int i = 0; i < 0x800; ++i) {
		data[i] = 0x00;
	}
}

uint8_t RAM::Read(uint16_t addr) {

	return data[addr];
}
void RAM::Write(uint16_t addr, uint8_t x) {
	data[addr] = x;
}

RAM::~RAM() {
	delete[] data;
}