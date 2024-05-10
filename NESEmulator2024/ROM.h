#pragma once

#include <cstdint>
#include <string>
#include <stdio.h>

class Bus;

class ROM
{
public:
	ROM(std::string filename, Bus* b);
	~ROM();
	
	uint8_t Read(uint16_t addr);
	void Write(uint16_t addr, uint8_t x);

	uint32_t getLength() { return length; }
	bool getEnd() { return end; }
private:
	bool end;
	uint8_t* data;
	uint32_t length;
	Bus* bus;
};