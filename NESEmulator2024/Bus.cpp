#include "Bus.h"
#include "CPU.h"
#include "RAM.h"
#include "ROM.h"
#include <iostream>
Bus::Bus() {
	cpu = new CPU(this);
	ram = new RAM(this);
	rom = new ROM("./roms/snake.bin", this);
	done = false;
	rom->Write(0xFFFC - 0x8000, 0x00);
	rom->Write(0xFFFD - 0x8000, 0x80);
}
Bus::~Bus() {

}
uint8_t	Bus::Read(uint16_t addr) {
	if (rom->getEnd()) {
		done = true;
	}
	if (addr < 0x1FFF && addr >= 0) {
		return ram->Read(addr & 0x07FF);
	}
	else if (addr >= 0x8000 && addr <= 0xFFFF) {
		return rom->Read(addr - 0x8000);
	}
	return 0x00;
}
bool Bus::getDone() { 
	return done;
}
void Bus::setDone(bool done) {
	this->done = done;
}
void Bus::Write(uint16_t addr, uint8_t data) {
	if (addr < 0x800 && addr >= 0) {
		ram->Write(addr, data);
	}
	else if (addr >= 0x8000 && addr <= 0xFFF) {
		rom->Write(addr - 0x8000, data);
		std::cout << "CANNOT WRITE TO ROM\n";
	}
}

uint8_t * Bus::getRAM() {
	return ram->data;
}