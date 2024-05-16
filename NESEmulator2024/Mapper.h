#pragma once


#include <cstdint>
#include "Bus.h"

class Mapper
{
public:
	Mapper(Bus * bus, Cartridge * cart) {
		this->bus = bus;
		this->cart = cart;
	}

	virtual uint8_t Read(uint16_t address) = 0;
	virtual void Write(uint16_t address, uint8_t data) = 0;
protected:
	Bus* bus;
	Cartridge* cart;
};

