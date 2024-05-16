#include "Cartridge.h"

#include <string>
#include <iostream>
#include "Bus.h"
#include "Mapper_0.h"



Cartridge::Cartridge(Bus* bus, std::string str) {
	uint8_t * data;
	unsigned int length;
	this->bus = bus;
	FILE* file;
	fopen_s(&file, str.c_str(), "rb");
	if (file == NULL) {
		std::cout << "ERROR: FAILED TO LOAD ROM FILE: " << str << "\n";
		length = 0;
		data = NULL;
		return;
	}
	fseek(file, 0, SEEK_END);
	length = ftell(file);
	rewind(file);

	data = new uint8_t[length];

	fread(data, length, 1, file);
	prgROMSize = 0;
	chrROM = NULL;
	chrROMSize = 0;
	
	if (data != NULL) {
		this->header = *((nes_header*)data);


		int romsize = 16384 * header.prgRomSize;
		prgROM = new uint8_t[romsize];
		uint8_t* romData = data + sizeof(nes_header);
		for (int i = 0; i < romsize; ++i) {
			prgROM[i] = romData[i];
		}
		prgROMSize = romsize;

		chrROMSize = 8192 * header.chrRomSize;
		chrROM = new uint8_t[chrROMSize];
		uint8_t* chrData = data + romsize + sizeof(nes_header);
		for (int i = 0; i < chrROMSize; ++i) {
			chrROM[i] = chrData[i];
		}

		delete[] data;
		printf("loaded cartridge\n");
	}
	else {
		header = nes_header();
	}

	mapper = new Mapper_0(bus, this);

	fclose(file);

}
void Cartridge::DrawPrgROM() {
	mem_edit.DrawWindow("PrgROM", prgROM, prgROMSize);
}