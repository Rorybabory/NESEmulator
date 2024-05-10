#include "CPU.h"
#include "Bus.h"
#include "Util.h"


#include <iostream>
#include <stdint.h>

#include "imgui/imgui.h"

static Oprand opcodes[] = {

	//0
	{&CPU::AddrImplied, &CPU::BRK, 7},			{&CPU::AddrIndexedIndirect, &CPU::ORA, 6},	{&CPU::AddrImplied, &CPU::NOP, 0},			{&CPU::AddrImplied, &CPU::NOP, 0},
	{&CPU::AddrImplied, &CPU::NOP, 3},			{&CPU::AddrZeroPage, &CPU::ORA, 3},			{&CPU::AddrZeroPage, &CPU::ASL, 5},			{&CPU::AddrImplied, &CPU::NOP, 5},
	{&CPU::AddrImplied, &CPU::PHP, 3},			{&CPU::AddrImmediate, &CPU::ORA, 2},		{&CPU::AddrAccumulator, &CPU::ASL, 2},		{&CPU::AddrImplied, &CPU::NOP, 2},
	{&CPU::AddrImplied, &CPU::NOP, 4},			{&CPU::AddrAbsolute, &CPU::ORA, 4},			{&CPU::AddrAbsolute, &CPU::ASL, 6},			{&CPU::AddrImplied, &CPU::NOP, 6},

	//1
	{&CPU::AddrRelative, &CPU::BPL, 2},			{&CPU::AddrIndirectIndexed, &CPU::ORA, 5},	{&CPU::AddrImplied, &CPU::NOP, 0},			{&CPU::AddrImplied, &CPU::NOP, 0},
	{&CPU::AddrImplied, &CPU::NOP, 0},			{&CPU::AddrZeroPageX, &CPU::ORA, 4},		{&CPU::AddrZeroPageX, &CPU::ASL, 6},		{&CPU::AddrImplied, &CPU::NOP, 6},
	{&CPU::AddrImplied, &CPU::CLC, 2},			{&CPU::AddrAbsoluteY, &CPU::ORA, 4},		{&CPU::AddrImplied, &CPU::NOP, 2},			{&CPU::AddrImplied, &CPU::NOP, 7},
	{&CPU::AddrImplied, &CPU::NOP, 4},			{&CPU::AddrAbsoluteX, &CPU::ORA, 4},		{&CPU::AddrAbsoluteX, &CPU::ASL, 7},		{&CPU::AddrImplied, &CPU::NOP, 7},

	//2
	{&CPU::AddrAbsolute, &CPU::JSR, 6},			{&CPU::AddrIndexedIndirect, &CPU::AND, 6},	{&CPU::AddrImplied, &CPU::NOP, 0},			{&CPU::AddrImplied, &CPU::NOP, 8},
	{&CPU::AddrZeroPage, &CPU::BIT, 3},			{&CPU::AddrZeroPage, &CPU::AND, 3},			{&CPU::AddrZeroPage, &CPU::ROL, 5},			{&CPU::AddrImplied, &CPU::NOP, 5},
	{&CPU::AddrImplied, &CPU::PLP, 4},			{&CPU::AddrImmediate, &CPU::AND, 2},		{&CPU::AddrAccumulator, &CPU::ROL, 2},		{&CPU::AddrImplied, &CPU::NOP, 2},
	{&CPU::AddrAbsolute, &CPU::BIT, 4},			{&CPU::AddrAbsolute, &CPU::AND, 4},			{&CPU::AddrAbsolute, &CPU::ROL, 6},			{&CPU::AddrImplied, &CPU::NOP, 6},

	//3
	{&CPU::AddrRelative, &CPU::BMI, 2},			{&CPU::AddrIndirectIndexed, &CPU::AND, 5},	{&CPU::AddrImplied, &CPU::NOP, 0},			{&CPU::AddrImplied, &CPU::NOP, 8},
	{&CPU::AddrImplied, &CPU::NOP, 4},			{&CPU::AddrZeroPageX, &CPU::AND, 4},		{&CPU::AddrZeroPageX, &CPU::ROL, 6},		{&CPU::AddrImplied, &CPU::NOP, 6},
	{&CPU::AddrImplied, &CPU::SEC, 2},			{&CPU::AddrAbsoluteY, &CPU::AND, 4},		{&CPU::AddrImplied, &CPU::NOP, 2},			{&CPU::AddrImplied, &CPU::NOP, 7},
	{&CPU::AddrImplied, &CPU::NOP, 4},			{&CPU::AddrAbsoluteX, &CPU::AND, 4},		{&CPU::AddrAbsoluteX, &CPU::ROL, 7},		{&CPU::AddrImplied, &CPU::NOP, 7},

	//4
	{&CPU::AddrImplied, &CPU::RTI, 6},			{&CPU::AddrIndexedIndirect, &CPU::EOR, 6},	{&CPU::AddrImplied, &CPU::NOP, 0},			{&CPU::AddrImplied, &CPU::NOP, 8},
	{&CPU::AddrImplied, &CPU::NOP, 3},			{&CPU::AddrZeroPage, &CPU::EOR, 3},			{&CPU::AddrZeroPage, &CPU::LSR, 5},			{&CPU::AddrImplied, &CPU::NOP, 5},
	{&CPU::AddrImplied, &CPU::PHA, 3},			{&CPU::AddrImmediate, &CPU::EOR, 2},		{&CPU::AddrAccumulator, &CPU::LSR, 2},		{&CPU::AddrImplied, &CPU::NOP, 2},
	{&CPU::AddrAbsolute, &CPU::JMP,3},			{&CPU::AddrAbsolute, &CPU::EOR, 4},			{&CPU::AddrAbsolute, &CPU::LSR, 6},			{&CPU::AddrImplied, &CPU::NOP, 6},

	//5
	{&CPU::AddrRelative, &CPU::BVC, 2},			{&CPU::AddrIndirectIndexed, &CPU::EOR, 5},	{&CPU::AddrImplied, &CPU::NOP, 0},			{&CPU::AddrImplied, &CPU::NOP, 8},
	{&CPU::AddrImplied, &CPU::NOP, 4},			{&CPU::AddrZeroPageX, &CPU::EOR, 4},		{&CPU::AddrZeroPageX, &CPU::LSR, 6},		{&CPU::AddrImplied, &CPU::NOP, 6},
	{&CPU::AddrImplied, &CPU::CLI, 2},			{&CPU::AddrAbsoluteX, &CPU::EOR, 4},		{&CPU::AddrImplied, &CPU::NOP, 2},			{&CPU::AddrImplied, &CPU::NOP, 7},
	{&CPU::AddrImplied, &CPU::NOP, 4},			{&CPU::AddrAbsoluteX, &CPU::EOR, 4},		{&CPU::AddrAbsoluteX, &CPU::LSR, 7},		{&CPU::AddrImplied, &CPU::NOP, 0},

	//6
	{&CPU::AddrImplied, &CPU::RTS, 6},			{&CPU::AddrIndexedIndirect, &CPU::ADC, 6},	{&CPU::AddrImplied, &CPU::NOP, 0},			{&CPU::AddrImplied, &CPU::NOP, 8},
	{&CPU::AddrImplied, &CPU::NOP, 3},			{&CPU::AddrZeroPage, &CPU::ADC, 3},			{&CPU::AddrZeroPage, &CPU::ROR, 5},			{&CPU::AddrImplied, &CPU::NOP, 5},
	{&CPU::AddrImplied, &CPU::PLA, 4},			{&CPU::AddrImmediate, &CPU::ADC, 2},		{&CPU::AddrAccumulator, &CPU::ROR, 2},		{&CPU::AddrImplied, &CPU::NOP, 2},
	{&CPU::AddrIndirect, &CPU::JMP, 5},			{&CPU::AddrAbsolute, &CPU::ADC, 4},			{&CPU::AddrAbsolute, &CPU::ROR, 6},			{&CPU::AddrImplied, &CPU::NOP, 6},

	//7
	{&CPU::AddrRelative, &CPU::BVS, 2},			{&CPU::AddrIndirectIndexed, &CPU::ADC, 5},	{&CPU::AddrImplied, &CPU::NOP, 0},			{&CPU::AddrImplied, &CPU::NOP, 8},
	{&CPU::AddrImplied, &CPU::NOP, 4},			{&CPU::AddrZeroPageX, &CPU::ADC, 4},		{&CPU::AddrZeroPageX, &CPU::ROR, 6},		{&CPU::AddrImplied, &CPU::NOP, 6},
	{&CPU::AddrImplied, &CPU::SEI, 2},			{&CPU::AddrAbsoluteY, &CPU::ADC, 4},		{&CPU::AddrImplied, &CPU::NOP, 2},			{&CPU::AddrImplied, &CPU::NOP, 7},
	{&CPU::AddrImplied, &CPU::NOP, 4},			{&CPU::AddrAbsoluteX, &CPU::ADC, 4},		{&CPU::AddrAbsoluteX, &CPU::ROR, 7},		{&CPU::AddrImplied, &CPU::NOP, 7},

	//8
	{&CPU::AddrImplied, &CPU::NOP, 2},			{&CPU::AddrIndexedIndirect, &CPU::STA, 6},	{&CPU::AddrImplied, &CPU::NOP, 2},			{&CPU::AddrImplied, &CPU::NOP, 6},
	{&CPU::AddrZeroPage, &CPU::STY, 3},			{&CPU::AddrZeroPage, &CPU::STA, 3},			{&CPU::AddrZeroPage, &CPU::STX, 3},			{&CPU::AddrImplied, &CPU::NOP, 3},
	{&CPU::AddrImplied, &CPU::DEY, 2},			{&CPU::AddrImplied, &CPU::NOP, 2},			{&CPU::AddrImplied, &CPU::TXA, 2},			{&CPU::AddrImplied, &CPU::NOP, 2},
	{&CPU::AddrAbsolute, &CPU::STY, 4},			{&CPU::AddrAbsolute, &CPU::STA, 4},			{&CPU::AddrAbsolute, &CPU::STX, 4},			{&CPU::AddrImplied, &CPU::NOP, 4},

	//9
	{&CPU::AddrRelative, &CPU::BCC, 2},			{&CPU::AddrIndirectIndexed, &CPU::STA, 6},	{&CPU::AddrImplied, &CPU::NOP, 0},			{&CPU::AddrImplied, &CPU::NOP, 6},
	{&CPU::AddrZeroPageX, &CPU::STY, 4},		{&CPU::AddrZeroPageX, &CPU::STA, 4},		{&CPU::AddrZeroPageY, &CPU::STX, 4},		{&CPU::AddrImplied, &CPU::NOP, 4},
	{&CPU::AddrImplied, &CPU::TYA, 2},			{&CPU::AddrAbsoluteY, &CPU::STA, 5},		{&CPU::AddrImplied, &CPU::TXS, 2},			{&CPU::AddrImplied, &CPU::NOP, 5},
	{&CPU::AddrImplied, &CPU::NOP, 5},			{&CPU::AddrAbsoluteX, &CPU::STA, 5},		{&CPU::AddrImplied, &CPU::NOP, 5},			{&CPU::AddrImplied, &CPU::NOP, 5},

	//A
	{&CPU::AddrImmediate, &CPU::LDY, 2},		{&CPU::AddrIndexedIndirect, &CPU::LDA, 6},	{&CPU::AddrImmediate, &CPU::LDX, 2},		{&CPU::AddrImplied, &CPU::NOP, 6},
	{&CPU::AddrZeroPage, &CPU::LDY, 3},			{&CPU::AddrZeroPage, &CPU::LDA, 3},			{&CPU::AddrZeroPage, &CPU::LDX, 3},			{&CPU::AddrImplied, &CPU::NOP, 3},
	{&CPU::AddrImplied, &CPU::TAY, 2},			{&CPU::AddrImmediate, &CPU::LDA, 2},		{&CPU::AddrImplied, &CPU::TAX, 2},			{&CPU::AddrImplied, &CPU::NOP, 2},
	{&CPU::AddrAbsolute, &CPU::LDY, 4},			{&CPU::AddrAbsolute, &CPU::LDA, 4},			{&CPU::AddrAbsolute, &CPU::LDX, 4},			{&CPU::AddrImplied, &CPU::NOP, 4},

	//B
	{&CPU::AddrRelative, &CPU::BCS, 2},			{&CPU::AddrIndirectIndexed, &CPU::LDA, 5},	{&CPU::AddrImplied, &CPU::NOP, 0},			{&CPU::AddrImplied, &CPU::NOP, 5},
	{&CPU::AddrZeroPageX, &CPU::LDY, 4},		{&CPU::AddrZeroPageX, &CPU::LDA, 4},		{&CPU::AddrZeroPageY, &CPU::LDX, 4},		{&CPU::AddrImplied, &CPU::NOP, 4},
	{&CPU::AddrImplied, &CPU::CLV, 2},			{&CPU::AddrAbsoluteY, &CPU::LDA, 4},		{&CPU::AddrImplied, &CPU::TSX, 2},			{&CPU::AddrImplied, &CPU::NOP, 4},
	{&CPU::AddrAbsoluteX, &CPU::LDY, 4},		{&CPU::AddrAbsoluteX, &CPU::LDA, 4},		{&CPU::AddrAbsoluteY, &CPU::LDX, 4},		{&CPU::AddrImplied, &CPU::NOP, 4},

	//C
	{&CPU::AddrImmediate, &CPU::CPY, 2},		{&CPU::AddrIndexedIndirect, &CPU::CMP, 6},	{&CPU::AddrImplied, &CPU::NOP, 2},			{&CPU::AddrImplied, &CPU::NOP, 8},
	{&CPU::AddrZeroPage, &CPU::CPY, 3},			{&CPU::AddrZeroPage, &CPU::CMP, 3},			{&CPU::AddrZeroPage, &CPU::DEC, 5},			{&CPU::AddrImplied, &CPU::NOP, 5},
	{&CPU::AddrImplied, &CPU::INY, 2},			{&CPU::AddrImmediate, &CPU::CMP, 2},		{&CPU::AddrImplied, &CPU::DEX, 2},			{&CPU::AddrImplied, &CPU::NOP, 2},
	{&CPU::AddrAbsolute, &CPU::CPY, 4},			{&CPU::AddrAbsolute, &CPU::CMP, 4},			{&CPU::AddrAbsolute, &CPU::DEC, 6},			{&CPU::AddrImplied, &CPU::NOP, 6},

	//D
	{&CPU::AddrRelative, &CPU::BNE, 2},			{&CPU::AddrIndirectIndexed, &CPU::CMP, 5},	{&CPU::AddrImplied, &CPU::NOP, 0},			{&CPU::AddrImplied, &CPU::NOP, 8},
	{&CPU::AddrImplied, &CPU::NOP, 4},			{&CPU::AddrZeroPageX, &CPU::CMP, 4},		{&CPU::AddrZeroPageX, &CPU::DEC, 6},		{&CPU::AddrImplied, &CPU::NOP, 6},
	{&CPU::AddrImplied, &CPU::CLD, 2},			{&CPU::AddrAbsoluteY, &CPU::CMP, 4},		{&CPU::AddrImplied, &CPU::NOP, 2},			{&CPU::AddrImplied, &CPU::NOP, 7},
	{&CPU::AddrImplied, &CPU::NOP, 4},			{&CPU::AddrAbsoluteX, &CPU::CMP, 4},		{&CPU::AddrAbsoluteX, &CPU::DEC, 7},		{&CPU::AddrImplied, &CPU::NOP, 7},

	//E
	{&CPU::AddrImmediate, &CPU::CPX, 2},		{&CPU::AddrIndexedIndirect, &CPU::SBC, 6},	{&CPU::AddrImplied, &CPU::NOP, 2},			{&CPU::AddrImplied, &CPU::NOP, 8},
	{&CPU::AddrZeroPage, &CPU::CPX, 3},			{&CPU::AddrZeroPage, &CPU::SBC, 3},			{&CPU::AddrZeroPage, &CPU::INC, 5},			{&CPU::AddrImplied, &CPU::NOP, 5},
	{&CPU::AddrImplied, &CPU::INX, 2},			{&CPU::AddrImmediate, &CPU::SBC, 2},		{&CPU::AddrImplied, &CPU::NOP, 2},			{&CPU::AddrImplied, &CPU::NOP, 2},
	{&CPU::AddrAbsolute, &CPU::CPX, 4},			{&CPU::AddrAbsolute, &CPU::SBC, 4},			{&CPU::AddrAbsolute, &CPU::INC, 6},			{&CPU::AddrImplied, &CPU::NOP, 6},

	//F
	{&CPU::AddrRelative, &CPU::BEQ, 2},			{&CPU::AddrIndirectIndexed, &CPU::SBC, 5},	{&CPU::AddrImplied, &CPU::NOP, 0},			{&CPU::AddrImplied, &CPU::NOP, 8},
	{&CPU::AddrImplied, &CPU::NOP, 4},			{&CPU::AddrZeroPageX, &CPU::SBC, 4},		{&CPU::AddrZeroPageX, &CPU::INC, 6},		{&CPU::AddrImplied, &CPU::NOP, 6},
	{&CPU::AddrImplied, &CPU::SED, 2},			{&CPU::AddrAbsoluteY, &CPU::SBC, 4},		{&CPU::AddrImplied, &CPU::NOP, 2},			{&CPU::AddrImplied, &CPU::NOP, 7},
	{&CPU::AddrImplied, &CPU::NOP, 4},			{&CPU::AddrAbsoluteX, &CPU::SBC, 4},		{&CPU::AddrAbsoluteX, &CPU::INC, 7},		{&CPU::AddrImplied, &CPU::NOP, 7}
};

CPU::CPU(Bus* b) { 
	this->bus = b;
	A = 0;
	X = 0;
	Y = 0;
	PC = 0x8000;
	FLAGS = 0;
	SP = 0xFF;
	opcode = 0x00;
	fetched = 0x00;
	address = 0x0000;
	delay = 0;
	cycles = 0;
	addrMode = MODE_IMPLIED;
}

CPU::~CPU() {

}
void CPU::SetFlag(uint8_t flag, bool val) {
	if (val == true) {
		FLAGS |= flag;
	}
	else {
		FLAGS &= ~flag;
	}
}
void CPU::DrawState() {
	ImGui::Begin("State");

	ImGui::SetWindowSize(ImVec2(350, 300));
	ImGui::SetWindowPos(ImVec2(1280 - 350, 75));

	ImGui::Text("A: %s X: %s Y: %s", Util::hex(A).c_str(), Util::hex(X).c_str(), Util::hex(Y).c_str());
	ImGui::Text("SP: %s PC: %s", Util::hex(SP).c_str(), Util::hex(PC).c_str());
	ImGui::Text("Opcode: %s", Util::hex(opcode).c_str());
	ImGui::End();

}

ImColor screenColors[] = {
	ImColor(0, 0, 0, 255),
	ImColor(255, 255, 255, 255),
	ImColor(136, 0, 0, 255),
	ImColor(170, 255, 238, 255),
	ImColor(204, 68, 204, 255),
	ImColor(0, 204, 85, 255),
	ImColor(0, 0, 170, 255),
	ImColor(238, 238, 119, 255),
	ImColor(221, 136, 85, 255),
	ImColor(102, 68, 0, 255),
	ImColor(255, 119, 119, 255),
	ImColor(51, 51, 51, 255),
	ImColor(119, 119, 119, 255),
	ImColor(170, 255, 102, 255),
	ImColor(0, 136, 255, 255),
	ImColor(187, 187, 187, 255)
};

void CPU::DrawScreen() {
	ImGui::Begin("Screen");

	ImGui::SetWindowSize(ImVec2(350, 350));
	ImGui::SetWindowPos(ImVec2(580, 75));
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	int size = 300;
	ImVec2 base = ImVec2(590, 105);
	for (int x = 0; x < 32; ++x) {
		for (int y = 0; y < 32; ++y) {
			ImColor color = screenColors[(bus->Read(0x200+32*y+x))%16];
			
			draw_list->AddRectFilled(ImVec2(base.x + x * (size / 32), base.y + y * (size / 32)), ImVec2(base.x + x * (size / 32) + size/32, base.y + y * (size / 32) + size / 32), color);
		}
	}

	ImGui::End();
}
void CPU::PrintState() {
	std::cout << "------------------------------------------------------------------------------------" << std::endl;
	std::cout << "A: " << Util::hex(A) << " X:" << Util::hex(X) << " Y:" << Util::hex(Y) << std::endl;
	std::cout << "SP: " << Util::hex(SP) << " PC:" << Util::hex(PC) << "\n";
	std::cout << "\opcode:" << Util::hex(opcode) << std::endl;
	std::cout << "0x0000:";
	for (uint16_t i = 0; i < 16; ++i) {
		std::cout << Util::hex(bus->Read(i)) << " ";
	}
	std::cout << "\n";
	std::cout << "0x01f0:";
	for (uint16_t i = 0; i < 16; ++i) {
		std::cout << Util::hex(bus->Read(i+0x01f0)) << " ";
	}
	std::cout << "\n";

	std::cout << "------------------------------------------------------------------------------------" << std::endl << std::endl;
}
void CPU::SetButton(uint8_t key) {
	bus->Write(0xFF, key);

}
bool CPU::Cycle() {
	if (delay > 0) {
		--delay;
		return false;
	}

	if (PC == 0x8081) {
		std::cout << "ILLEGAL MOVE\n";
	}

	bus->Write(0xFE, (uint8_t)rand());

	opcode = bus->Read(PC);
	
	delay = opcodes[opcode].delay - 1;

	//PrintState();

	address = opcodes[opcode].addrMode(*this); // call addressing mode

	opcodes[opcode].instruction(*this); // call instruction

	++PC;
	++cycles;

	//if (cycles > 100) {
	//	std::cout << "TOO MANY CYCLES\n";
	//	bus->setDone(true);
	//}

	return true;
}

void CPU::RestartClear() {
	A = 0;
	X = 0;
	Y = 0;
	PC = 0x8000;
	FLAGS = 0;
	SP = 0xFF;
	opcode = 0x00;
	fetched = 0x00;
	address = 0x0000;
	delay = 0;
	cycles = 0;
	addrMode = MODE_IMPLIED;
	bus->setDone(false);
	for (int i = 0; i < 0x800; ++i) {
		bus->Write(i, 0x00);
	}
}

void CPU::Reset() {
	A = 0;
	X = 0;
	Y = 0;
	SP = 0xFD;
	FLAGS = 0;

	PC = (bus->Read(0xFFFD) << 8) | bus->Read(0xFFFC);
	delay = 8;
}
void CPU::IRQ() {
	if ((FLAGS & FLAG_IRQ) == 0) {
		bus->Write(0x100 + SP, (PC >> 8) & 0x00FF);
		--SP;
		bus->Write(0x100 + SP, (PC) & 0x00FF);
		--SP;

		SetFlag(FLAG_UNUSED, 1);
		SetFlag(FLAG_IRQ, 1);
		SetFlag(FLAG_BRK, 0);
		bus->Write(0x0100 + SP, FLAGS);
		--SP;

		PC = (bus->Read(0xFFFF) << 8) | bus->Read(0xFFFE);
		delay = 7;
	}
}

void CPU::NMI() {
	bus->Write(0x100 + SP, (PC >> 8) & 0x00FF);
	--SP;
	bus->Write(0x100 + SP, (PC) & 0x00FF);
	--SP;

	SetFlag(FLAG_UNUSED, 1);
	SetFlag(FLAG_IRQ, 1);
	SetFlag(FLAG_BRK, 0);
	bus->Write(0x0100 + SP, FLAGS);
	--SP;

	PC = (bus->Read(0xFFFF) << 8) | bus->Read(0xFFFE);
	delay = 7;
}

//------Addressing Modes-------
uint16_t CPU::AddrImplied() {
	addrMode = MODE_IMPLIED;
	return 0;
}
uint16_t CPU::AddrAccumulator() {
	addrMode = MODE_ACCUMULATOR;
	return 0;
}
uint16_t CPU::AddrImmediate() {
	addrMode = MODE_IMMEDIATE;

	++PC;
	return 0;
}
uint16_t CPU::AddrZeroPage() {
	addrMode = MODE_ZEROPAGE;
	
	++PC;
	return bus->Read(PC);
}
uint16_t CPU::AddrZeroPageX() {
	addrMode = MODE_ZEROPAGEX;
	++PC;
	return bus->Read(PC) + X;
}
uint16_t CPU::AddrZeroPageY() {
	addrMode = MODE_ZEROPAGEY;
	++PC;
	return bus->Read(PC) + Y;
}
uint16_t CPU::AddrRelative() {
	addrMode = MODE_RELATIVE;
	++PC;
	uint8_t offset = bus->Read(PC);
	if (offset > 127) {
		offset = ~offset + 1;
		return PC - offset;
	}
	else {
		return PC + offset;
	}
}
uint16_t CPU::AddrAbsolute() {
	addrMode = MODE_ABSOLUTE;
	++PC;
	uint8_t low = bus->Read(PC);
	++PC;
	uint8_t high = bus->Read(PC);
	
	return ((high << 8) + low);
}
uint16_t CPU::AddrAbsoluteX() {
	addrMode = MODE_ABSOLUTEX;

	++PC;
	uint8_t low = bus->Read(PC);
	++PC;
	uint8_t high = bus->Read(PC);

	uint16_t result = ((high << 8) + low) + X;

	if ((result & 0xFF00) != (high << 8)) {
		++delay;
	}

	return result;
}
uint16_t CPU::AddrAbsoluteY() {
	addrMode = MODE_ABSOLUTEY;

	++PC;
	uint8_t low = bus->Read(PC);
	++PC;
	uint8_t high = bus->Read(PC);

	return ((high << 8) + low) + Y;
}
uint16_t CPU::AddrIndirect() {
	addrMode = MODE_INDIRECT;
	++PC;
	uint8_t low = bus->Read(PC);
	++PC;
	uint8_t high = bus->Read(PC);

	uint8_t low2 = bus->Read((high << 8) + low);
	uint8_t high2 = bus->Read((high << 8) + low+1);

	return (high2 << 8) + low2;
}
uint16_t CPU::AddrIndexedIndirect() {
	addrMode = MODE_INDEXEDINDIRECT;

	++PC;
	uint16_t zp = bus->Read(PC);
	zp += X;
	uint8_t low = bus->Read(zp);
	uint8_t high = bus->Read(zp + 1);

	return (high << 8) + low;
}
uint16_t CPU::AddrIndirectIndexed() {
	addrMode = MODE_INDIRECTINDEXED;
	++PC;
	uint16_t zp = bus->Read(PC);

	uint8_t low = bus->Read(zp);
	uint8_t high = bus->Read(zp + 1);

	return (high << 8) + low + Y;
}


void CPU::ADC() {
	uint16_t value = 0;
	if (addrMode == MODE_IMMEDIATE) {
		value = bus->Read(PC);
	} else {
		value = bus->Read(address);
	}
	uint16_t result = (uint8_t)A + value + ((FLAGS & FLAG_CARRY) * 128);

	
	SetFlag(FLAG_OVERFLOW, (value & 128) == ((A & 128)) && (value & 128) != (result & 128));

	A = (uint8_t)result;
	
	SetFlag(FLAG_CARRY, result > 255);
	SetFlag(FLAG_NEGATIVE, (A & 128) == 128);
	SetFlag(FLAG_ZERO, A == 0);
}
void CPU::AND() {
	uint8_t value = 0;
	if (addrMode == MODE_IMMEDIATE) {
		value = bus->Read(PC);
	}
	else {
		value = bus->Read(address);
	}

	
	A &= value;

	SetFlag(FLAG_NEGATIVE, (A & 128) == 128);
	SetFlag(FLAG_ZERO, A == 0);
}
void CPU::ASL() {
	uint16_t tmp = 0;

	if (addrMode == MODE_ACCUMULATOR) {
		tmp = A << 1;
		A = tmp & 0x00FF;
	}
	else {
		tmp = bus->Read(address) << 1;
		bus->Write(address, tmp & 0x00FF);
	}

	SetFlag(FLAG_CARRY, (tmp & 0xFF00) > 255);
	SetFlag(FLAG_NEGATIVE, (tmp & 128) == 128);
	SetFlag(FLAG_ZERO, (tmp & 0x00FF) == 0);

}
void CPU::BCC() {
	if ((FLAGS & FLAG_CARRY) == 0) {
		PC = address;
	}
}
void CPU::BCS() {
	if ((FLAGS & FLAG_CARRY) != 0) {
		PC = address;
	}
}
void CPU::BEQ() {
	if ((FLAGS & FLAG_ZERO) != 0) {
		PC = address;
	}
}
void CPU::BIT() {
	uint8_t value = 0;
	if (addrMode == MODE_IMMEDIATE) {
		value = bus->Read(PC);
	}
	else {
		value = bus->Read(address);
	}
	uint8_t result = A & value;


	SetFlag(FLAG_NEGATIVE, (value & 128) == 128);
	SetFlag(FLAG_OVERFLOW, (value & 64) == 64);
	SetFlag(FLAG_ZERO, result == 0);

}
void CPU::BMI() {
	if ((FLAGS & FLAG_NEGATIVE) != 0) {
		PC = address;
	}
}
void CPU::BNE() {
	if ((FLAGS & FLAG_ZERO) == 0) {
		PC = address;
	}
}
void CPU::BPL() {
	if ((FLAGS & FLAG_NEGATIVE) == 0) {
		PC = address;
	}
}
void CPU::BRK() {

	bus->setDone(true);
}
void CPU::BVC() {
	if ((FLAGS & FLAG_OVERFLOW) == 0) {
		PC = address;
	}
}
void CPU::BVS() {
	if ((FLAGS & FLAG_OVERFLOW) != 0) {
		PC = address;
	}
}
void CPU::CLC() {
	//FLAGS &= ~FLAG_CARRY;
	SetFlag(FLAG_CARRY, false);
}
void CPU::CLD() {
	FLAGS &= ~FLAG_DECIMAL;
}
void CPU::CLI() {
	FLAGS &= ~FLAG_IRQ;

}
void CPU::CLV() {
	FLAGS &= ~FLAG_OVERFLOW;
}
void CPU::CMP() {
	uint16_t value = 0;
	if (addrMode == MODE_IMMEDIATE) {
		value = bus->Read(PC);
	}
	else {
		value = bus->Read(address);
	}
	uint16_t result = (uint16_t)A - value;


	SetFlag(FLAG_CARRY, A >= value);
	SetFlag(FLAG_NEGATIVE, (result & 128) == 128);
	SetFlag(FLAG_ZERO, (result & 0x00FF) == 0);
}
void CPU::CPX() {
	uint16_t value = 0;
	if (addrMode == MODE_IMMEDIATE) {
		value = bus->Read(PC);
	}
	else {
		value = bus->Read(address);
	}
	uint16_t result = (uint16_t)X - value;


	SetFlag(FLAG_CARRY, X >= value);
	SetFlag(FLAG_NEGATIVE, (result & 128) == 128);
	SetFlag(FLAG_ZERO, (result & 0x00FF) == 0);
}
void CPU::CPY() {
	uint16_t value = 0;
	if (addrMode == MODE_IMMEDIATE) {
		value = bus->Read(PC);
	}
	else {
		value = bus->Read(address);
	}
	uint16_t result = (uint16_t)Y - value;


	SetFlag(FLAG_CARRY, Y >= value);
	SetFlag(FLAG_NEGATIVE, (result & 128) == 128);
	SetFlag(FLAG_ZERO, (result & 0x00FF) == 0);
}
void CPU::DEC() {
	bus->Write(address, bus->Read(address) - 1);
	SetFlag(FLAG_NEGATIVE, (bus->Read(address) & 128) == 128);
	SetFlag(FLAG_ZERO, bus->Read(address) == 0);
}
void CPU::DEX() {
	--X;
	SetFlag(FLAG_NEGATIVE, (X & 128) == 128);
	SetFlag(FLAG_ZERO, X == 0);

}
void CPU::DEY() {
	--Y;
	SetFlag(FLAG_NEGATIVE, (Y & 128) == 128);
	SetFlag(FLAG_ZERO, Y == 0);
}
void CPU::EOR() {
	uint8_t value = 0;
	if (addrMode == MODE_IMMEDIATE) {
		value = bus->Read(PC);
	}
	else {
		value = bus->Read(address);
	}


	A ^= value;

	SetFlag(FLAG_NEGATIVE, (A & 128) == 128);
	SetFlag(FLAG_ZERO, A == 0);
}
void CPU::INC() {
	bus->Write(address, bus->Read(address) + 1);
	SetFlag(FLAG_NEGATIVE, (bus->Read(address) & 128) == 128);
	SetFlag(FLAG_ZERO, bus->Read(address) == 0);
}
void CPU::INX() {
	++X;
	SetFlag(FLAG_NEGATIVE, (X & 128) == 128);
	SetFlag(FLAG_ZERO, X == 0);
}
void CPU::INY() {
	++Y;
	SetFlag(FLAG_NEGATIVE, (Y & 128) == 128);
	SetFlag(FLAG_ZERO, Y == 0);
}
void CPU::JMP() {
	PC = address - 1;
}
void CPU::JSR() {
	bus->Write(0x0100 + SP, (PC >> 8) & 0x00FF);
	--SP;
	bus->Write(0x0100 + SP, PC & 0x00FF);
	--SP;
	PC = address - 1;

}
void CPU::LDA() {
	if (addrMode == MODE_IMMEDIATE) {
		A = bus->Read(PC);
	} else {
		A = bus->Read(address);
	}
	if (A == 0) {
		FLAGS |= FLAG_ZERO;
	}
	if ((A & 128) == 128) {
		FLAGS |= FLAG_NEGATIVE;
	}
}
void CPU::LDX() {
	if (addrMode == MODE_IMMEDIATE) {
		X = bus->Read(PC);
	} else {
		X = bus->Read(address);
	}
	if (X == 0) {
		FLAGS |= FLAG_ZERO;
	}
	if ((X & 128) == 128) {
		FLAGS |= FLAG_NEGATIVE;
	}
}
void CPU::LDY() {
	if (addrMode == MODE_IMMEDIATE) {
		Y = bus->Read(PC);
	}
	else {
		Y = bus->Read(address);
	}
	if (Y == 0) {
		FLAGS |= FLAG_ZERO;
	}
	if ((Y & 128) == 128) {
		FLAGS |= FLAG_NEGATIVE;
	}
}
void CPU::LSR() {
	uint16_t tmp = 0;

	if (addrMode == MODE_ACCUMULATOR) {
		tmp = A >> 1;
		SetFlag(FLAG_CARRY, A & 1);
		A = (uint8_t)tmp;

	}
	else {
		tmp = bus->Read(address) >> 1;
		SetFlag(FLAG_CARRY, bus->Read(address) & 1);
		bus->Write(address, tmp & 0x00FF);
	}

	SetFlag(FLAG_NEGATIVE, (tmp & 256) == 256);
	SetFlag(FLAG_ZERO, tmp == 0);
}
void CPU::NOP() {

}
void CPU::ORA() {
	uint8_t value = 0;
	if (addrMode == MODE_IMMEDIATE) {
		value = bus->Read(PC);
	}
	else {
		value = bus->Read(address);
	}


	A |= value;

	SetFlag(FLAG_NEGATIVE, (A & 128) == 128);
	SetFlag(FLAG_ZERO, A == 0);
}
void CPU::PHA() {
	bus->Write(0x0100 + SP, A);
	--SP;
}
void CPU::PHP() {
	bus->Write(0x0100 + SP, FLAGS);
	--SP;
}
void CPU::PLA() {
	++SP;
	A = bus->Read(0x0100 + SP);
	SetFlag(FLAG_ZERO, A == 0);
	SetFlag(FLAG_NEGATIVE, (A & 128) == 128);
}
void CPU::PLP() {
	++SP;
	FLAGS = bus->Read(0x0100 + SP);
	SetFlag(FLAG_ZERO, FLAGS == 0);
	SetFlag(FLAG_NEGATIVE, (FLAGS & 128) == 128);

}
void CPU::ROL() {
	uint16_t tmp = 0;

	if (addrMode == MODE_ACCUMULATOR) {
		tmp = A << 1 | ((FLAGS & FLAG_CARRY) << 7);
		A = (uint8_t)tmp;
	}
	else {
		tmp = bus->Read(address) << 1 | (FLAGS & FLAG_CARRY);
		bus->Write(address, tmp & 0x00FF);
	}

	SetFlag(FLAG_CARRY, tmp & 0xFF00);
	SetFlag(FLAG_NEGATIVE, (tmp & 128) == 128);
	SetFlag(FLAG_ZERO, (tmp & 0x00FF) == 0);
}
void CPU::ROR() {
	uint16_t tmp = 0;

	if (addrMode == MODE_ACCUMULATOR) {
		tmp = A >> 1 | ((FLAGS & FLAG_CARRY) << 7);
		A = (uint8_t)tmp;
	}
	else {
		tmp = bus->Read(address) >> 1 | ((FLAGS & FLAG_CARRY) << 7);
		bus->Write(address, tmp & 0x00FF);
	}

	SetFlag(FLAG_CARRY, tmp & 0x01);
	SetFlag(FLAG_NEGATIVE, (tmp & 128) == 128);
	SetFlag(FLAG_ZERO, (tmp & 0x00FF) == 0);
}
void CPU::RTI() {
	++SP;
	FLAGS = bus->Read(0x0100 + SP);

	SetFlag(FLAG_BRK, 0);
	SetFlag(FLAG_UNUSED, 0);

	++SP;
	PC = (uint16_t)bus->Read(0x0100 + SP);
	++SP;
	PC |= (uint16_t)bus->Read(0x0100 + SP) << 8;

}
void CPU::RTS() {
	++SP;

	PC = bus->Read(0x0100 + SP);
	++SP;
	PC |= bus->Read(0x0100 + SP) << 8;

}
void CPU::SBC() {
	uint16_t value = 0;
	if (addrMode == MODE_IMMEDIATE) {
		value = bus->Read(PC);
	}
	else {
		value = bus->Read(address);
	}
	uint16_t result = (uint8_t)A + (value ^ 0x00FF) + (FLAGS & FLAG_CARRY);


	SetFlag(FLAG_OVERFLOW, (value & 128) == ((A & 128)) && (value & 128) != (result & 128));

	A = (uint8_t)result;

	SetFlag(FLAG_CARRY, result > 255);
	SetFlag(FLAG_NEGATIVE, (A & 128) == 128);
	SetFlag(FLAG_ZERO, A == 0);
}
void CPU::SEC() {
	SetFlag(FLAG_CARRY, true);
}
void CPU::SED() {
	SetFlag(FLAG_DECIMAL, true);
}
void CPU::SEI() {
	SetFlag(FLAG_IRQ, true);
}
void CPU::STA() {
	bus->Write(address, A);
}
void CPU::STX() {
	bus->Write(address, X);
}
void CPU::STY() {
	bus->Write(address, Y);
}
void CPU::TAX() {
	X = A;
}
void CPU::TAY() {
	Y = A;
}
void CPU::TSX() {
	X = bus->Read(0x0100 + SP);
}
void CPU::TXA() {
	A = X;
}
void CPU::TXS() {
	bus->Write(0x0100 + SP, X);
}
void CPU::TYA() {
	A = Y;
}
