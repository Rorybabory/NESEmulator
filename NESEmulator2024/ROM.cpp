#include "ROM.h"
#include <iostream>

ROM::ROM(std::string filename, Bus* b) {
	this->bus = b;
	this->end = false;
	FILE* file;
	fopen_s(&file, filename.c_str(), "rb");
	if (file == NULL) {
		std::cout << "ERROR: FAILED TO LOAD ROM FILE: " << filename << "\n";
		length = 0;
		data = NULL;
		return;
	}
	fseek(file, 0, SEEK_END);
	length = ftell(file);
	rewind(file);

	data = new uint8_t[0x8000];
	if (length > 0x8000) {
		std::cout << "ERROR: ROM FILE TOO LARGE" << "\n";
		return;
	}
	fread(data, length, 1, file);
	fclose(file);

}
ROM::~ROM() {
	delete[] data;
}

uint8_t ROM::Read(uint16_t addr) {
	if (addr < length) {
		return data[addr];
	}
	else {
		end = true;
		return 0x00;
	}

}

void ROM::Write(uint16_t addr, uint8_t x) {
	data[addr] = x;
}
