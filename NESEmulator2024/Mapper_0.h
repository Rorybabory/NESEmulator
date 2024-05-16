#pragma once
#include "Mapper.h"
#include <iostream>
class Mapper_0 :
    public Mapper
{
public:
    Mapper_0(Bus* bus, Cartridge * cart) : Mapper(bus, cart) {
        std::cout << "Using Mapper 0\n";
    }
    uint8_t Read(uint16_t address);
    void Write(uint16_t address, uint8_t data);
};

