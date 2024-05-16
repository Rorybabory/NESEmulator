#pragma once

#include <string>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_memory_editor.h"


class Bus;
class Mapper;

typedef struct {
	char ID[4];
	uint8_t prgRomSize; //size = 16k * prgRomSize
	uint8_t chrRomSize;//size = 8k * prgRomSize

	uint8_t flags6;
	uint8_t flags7;

	uint8_t mapperInfo;

	uint8_t sizeMSB;
	uint8_t chrRamSize;

	uint8_t timing;

	uint8_t type;

	uint8_t miscellaneousRoms;

	uint8_t expansionDevice;

	uint8_t padding;

}nes_header;


class Cartridge
{
public:
	Cartridge(Bus * bus, std::string str);

	void DrawPrgROM();
	
	Mapper* mapper;
	uint8_t* prgROM;
	int prgROMSize;
	
	uint8_t* chrROM;
	int chrROMSize;
private:
	MemoryEditor mem_edit;
	nes_header header;
	Bus* bus;
};
