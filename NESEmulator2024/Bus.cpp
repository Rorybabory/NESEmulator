#include "Bus.h"
#include "CPU.h"
#include "RAM.h"

#include "Cartridge.h"
#include "Mapper.h"
#include <iostream>
Bus::Bus() {
	cpu = new CPU(this);
	ram = new RAM(this);
	cart = new Cartridge(this, "./roms/cart2.nes");
	done = false;
	//cart->mapper->Write(0xFFFC, 0x00);
	//cart->mapper->Write(0xFFFD, 0x80);
}
Bus::~Bus() {

}
uint8_t	Bus::Read(uint16_t addr) {
	if (addr < 0x1FFF && addr >= 0) {
		return ram->Read(addr & 0x07FF);
	}
	if (addr >= 0x4020) {
		return cart->mapper->Read(addr);
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
	if (addr < 0x1FFF && addr >= 0) {
		ram->Write(addr & 0x07FF, data);
	}
	if (addr >= 0x4020) {
		cart->mapper->Write(addr, data);
	}
}

uint8_t * Bus::getRAM() {
	return ram->data;
}