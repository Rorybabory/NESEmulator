#include "CPU.h"
#include "Bus.h"
#include "Util.h"


#include <iostream>
#include <stdint.h>
#include <sstream>
#include <string>

#include "imgui/imgui.h"

static std::function<uint16_t(CPU&)> addrMap[] = {
	&CPU::AddrImplied,
	&CPU::AddrAccumulator, 
	&CPU::AddrImmediate,
	&CPU::AddrZeroPage,
	&CPU::AddrZeroPageX,
	&CPU::AddrZeroPageY,
	&CPU::AddrRelative,
	&CPU::AddrAbsolute,
	&CPU::AddrAbsoluteX,
	&CPU::AddrAbsoluteY,
	&CPU::AddrIndirect,
	&CPU::AddrIndexedIndirect,
	&CPU::AddrIndirectIndexed,
};

static Oprand opcodes[] = {

	//0
	{MODE_IMPLIED, &CPU::BRK, "BRK", 7},			{MODE_INDEXEDINDIRECT, &CPU::ORA, "ORA", 6},	{MODE_IMPLIED, &CPU::NOP, "NOP", 0},			{MODE_IMPLIED, &CPU::NOP, "NOP", 0},
	{MODE_IMPLIED, &CPU::NOP, "NOP", 3},			{MODE_ZEROPAGE, &CPU::ORA, "ORA", 3},			{MODE_ZEROPAGE, &CPU::ASL, "ASL", 5},			{MODE_IMPLIED, &CPU::NOP, "NOP", 5},
	{MODE_IMPLIED, &CPU::PHP, "PHP", 3},			{MODE_IMMEDIATE, &CPU::ORA, "ORA", 2},		{MODE_ACCUMULATOR, &CPU::ASL, "ASL", 2},		{MODE_IMPLIED, &CPU::NOP, "NOP", 2},
	{MODE_IMPLIED, &CPU::NOP, "NOP", 4},			{MODE_ABSOLUTE, &CPU::ORA, "ORA", 4},			{MODE_ABSOLUTE, &CPU::ASL, "ASL", 6},			{MODE_IMPLIED, &CPU::NOP, "NOP", 6},

	//1
	{MODE_RELATIVE, &CPU::BPL, "BPL", 2},			{MODE_INDIRECTINDEXED, &CPU::ORA, "ORA", 5},	{MODE_IMPLIED, &CPU::NOP, "NOP", 0},			{MODE_IMPLIED, &CPU::NOP, "NOP", 0},
	{MODE_IMPLIED, &CPU::NOP, "NOP", 0},			{MODE_ZEROPAGEX, &CPU::ORA, "ORA", 4},		{MODE_ZEROPAGEX, &CPU::ASL, "ASL", 6},		{MODE_IMPLIED, &CPU::NOP, "NOP", 6},
	{MODE_IMPLIED, &CPU::CLC, "CLC", 2},			{MODE_ABSOLUTEY, &CPU::ORA, "ORA", 4},		{MODE_IMPLIED, &CPU::NOP, "NOP", 2},			{MODE_IMPLIED, &CPU::NOP, "NOP", 7},
	{MODE_IMPLIED, &CPU::NOP, "NOP", 4},			{MODE_ABSOLUTEX, &CPU::ORA, "ORA", 4},		{MODE_ABSOLUTEX, &CPU::ASL, "ASL", 7},		{MODE_IMPLIED, &CPU::NOP, "NOP", 7},

	//2
	{MODE_ABSOLUTE, &CPU::JSR, "JSR", 6},			{MODE_INDEXEDINDIRECT, &CPU::AND, "AND", 6},	{MODE_IMPLIED, &CPU::NOP, "NOP", 0},			{MODE_IMPLIED, &CPU::NOP, "NOP", 8},
	{MODE_ZEROPAGE, &CPU::BIT, "BIT", 3},			{MODE_ZEROPAGE, &CPU::AND, "AND", 3},			{MODE_ZEROPAGE, &CPU::ROL, "ROL", 5},			{MODE_IMPLIED, &CPU::NOP, "NOP", 5},
	{MODE_IMPLIED, &CPU::PLP, "PLP", 4},			{MODE_IMMEDIATE, &CPU::AND, "AND", 2},		{MODE_ACCUMULATOR, &CPU::ROL, "ROL", 2},		{MODE_IMPLIED, &CPU::NOP, "NOP", 2},
	{MODE_ABSOLUTE, &CPU::BIT, "BIT", 4},			{MODE_ABSOLUTE, &CPU::AND, "AND", 4},			{MODE_ABSOLUTE, &CPU::ROL, "ROL", 6},			{MODE_IMPLIED, &CPU::NOP, "NOP", 6},

	//3
	{MODE_RELATIVE, &CPU::BMI, "BMI", 2},			{MODE_INDIRECTINDEXED, &CPU::AND, "AND", 5},	{MODE_IMPLIED, &CPU::NOP, "NOP", 0},			{MODE_IMPLIED, &CPU::NOP, "NOP", 8},
	{MODE_IMPLIED, &CPU::NOP, "NOP", 4},			{MODE_ZEROPAGEX, &CPU::AND, "AND", 4},		{MODE_ZEROPAGEX, &CPU::ROL, "ROL", 6},		{MODE_IMPLIED, &CPU::NOP, "NOP", 6},
	{MODE_IMPLIED, &CPU::SEC, "SEC", 2},			{MODE_ABSOLUTEY, &CPU::AND, "AND", 4},		{MODE_IMPLIED, &CPU::NOP, "NOP", 2},			{MODE_IMPLIED, &CPU::NOP, "NOP", 7},
	{MODE_IMPLIED, &CPU::NOP, "NOP", 4},			{MODE_ABSOLUTEX, &CPU::AND, "AND", 4},		{MODE_ABSOLUTEX, &CPU::ROL, "ROL", 7},		{MODE_IMPLIED, &CPU::NOP, "NOP", 7},

	//4
	{MODE_IMPLIED, &CPU::RTI, "RTI", 6},			{MODE_INDEXEDINDIRECT, &CPU::EOR, "EOR", 6},	{MODE_IMPLIED, &CPU::NOP, "NOP", 0},			{MODE_IMPLIED, &CPU::NOP, "NOP", 8},
	{MODE_IMPLIED, &CPU::NOP, "NOP", 3},			{MODE_ZEROPAGE, &CPU::EOR, "EOR", 3},			{MODE_ZEROPAGE, &CPU::LSR, "LSR", 5},			{MODE_IMPLIED, &CPU::NOP, "NOP", 5},
	{MODE_IMPLIED, &CPU::PHA, "PHA", 3},			{MODE_IMMEDIATE, &CPU::EOR, "EOR", 2},		{MODE_ACCUMULATOR, &CPU::LSR, "LSR", 2},		{MODE_IMPLIED, &CPU::NOP, "NOP", 2},
	{MODE_ABSOLUTE, &CPU::JMP, "JMP",3},			{MODE_ABSOLUTE, &CPU::EOR, "EOR", 4},			{MODE_ABSOLUTE, &CPU::LSR, "LSR", 6},			{MODE_IMPLIED, &CPU::NOP, "NOP", 6},

	//5
	{MODE_RELATIVE, &CPU::BVC, "BVC", 2},			{MODE_INDIRECTINDEXED, &CPU::EOR, "EOR", 5},	{MODE_IMPLIED, &CPU::NOP, "NOP", 0},			{MODE_IMPLIED, &CPU::NOP, "NOP", 8},
	{MODE_IMPLIED, &CPU::NOP, "NOP", 4},			{MODE_ZEROPAGEX, &CPU::EOR, "EOR", 4},		{MODE_ZEROPAGEX, &CPU::LSR, "LSR", 6},		{MODE_IMPLIED, &CPU::NOP, "NOP", 6},
	{MODE_IMPLIED, &CPU::CLI, "CLI", 2},			{MODE_ABSOLUTEX, &CPU::EOR, "EOR", 4},		{MODE_IMPLIED, &CPU::NOP, "NOP", 2},			{MODE_IMPLIED, &CPU::NOP, "NOP", 7},
	{MODE_IMPLIED, &CPU::NOP, "NOP", 4},			{MODE_ABSOLUTEX, &CPU::EOR, "EOR", 4},		{MODE_ABSOLUTEX, &CPU::LSR, "LSR", 7},		{MODE_IMPLIED, &CPU::NOP, "NOP", 0},

	//6
	{MODE_IMPLIED, &CPU::RTS, "RTS", 6},			{MODE_INDEXEDINDIRECT, &CPU::ADC, "ADC", 6},	{MODE_IMPLIED, &CPU::NOP, "NOP", 0},			{MODE_IMPLIED, &CPU::NOP, "NOP", 8},
	{MODE_IMPLIED, &CPU::NOP, "NOP", 3},			{MODE_ZEROPAGE, &CPU::ADC, "ADC", 3},			{MODE_ZEROPAGE, &CPU::ROR, "ROR", 5},			{MODE_IMPLIED, &CPU::NOP, "NOP", 5},
	{MODE_IMPLIED, &CPU::PLA, "PLA", 4},			{MODE_IMMEDIATE, &CPU::ADC, "ADC", 2},		{MODE_ACCUMULATOR, &CPU::ROR, "ROR", 2},		{MODE_IMPLIED, &CPU::NOP, "NOP", 2},
	{MODE_INDIRECT, &CPU::JMP, "JMP", 5},			{MODE_ABSOLUTE, &CPU::ADC, "ADC", 4},			{MODE_ABSOLUTE, &CPU::ROR, "ROR", 6},			{MODE_IMPLIED, &CPU::NOP, "NOP", 6},

	//7
	{MODE_RELATIVE, &CPU::BVS, "BVS", 2},			{MODE_INDIRECTINDEXED, &CPU::ADC, "ADC", 5},	{MODE_IMPLIED, &CPU::NOP, "NOP", 0},			{MODE_IMPLIED, &CPU::NOP, "NOP", 8},
	{MODE_IMPLIED, &CPU::NOP, "NOP", 4},			{MODE_ZEROPAGEX, &CPU::ADC, "ADC", 4},		{MODE_ZEROPAGEX, &CPU::ROR, "ROR", 6},		{MODE_IMPLIED, &CPU::NOP, "NOP", 6},
	{MODE_IMPLIED, &CPU::SEI, "SEI", 2},			{MODE_ABSOLUTEY, &CPU::ADC, "ADC", 4},		{MODE_IMPLIED, &CPU::NOP, "NOP", 2},			{MODE_IMPLIED, &CPU::NOP, "NOP", 7},
	{MODE_IMPLIED, &CPU::NOP, "NOP", 4},			{MODE_ABSOLUTEX, &CPU::ADC, "ADC", 4},		{MODE_ABSOLUTEX, &CPU::ROR, "ROR", 7},		{MODE_IMPLIED, &CPU::NOP, "NOP", 7},

	//8
	{MODE_IMPLIED, &CPU::NOP, "NOP", 2},			{MODE_INDEXEDINDIRECT, &CPU::STA, "STA", 6},	{MODE_IMPLIED, &CPU::NOP, "NOP", 2},			{MODE_IMPLIED, &CPU::NOP, "NOP", 6},
	{MODE_ZEROPAGE, &CPU::STY, "STY", 3},			{MODE_ZEROPAGE, &CPU::STA, "STA", 3},			{MODE_ZEROPAGE, &CPU::STX, "STX", 3},			{MODE_IMPLIED, &CPU::NOP, "NOP", 3},
	{MODE_IMPLIED, &CPU::DEY, "DEY", 2},			{MODE_IMPLIED, &CPU::NOP, "NOP", 2},			{MODE_IMPLIED, &CPU::TXA, "TXA", 2},			{MODE_IMPLIED, &CPU::NOP, "NOP", 2},
	{MODE_ABSOLUTE, &CPU::STY, "STY", 4},			{MODE_ABSOLUTE, &CPU::STA, "STA", 4},			{MODE_ABSOLUTE, &CPU::STX, "STX", 4},			{MODE_IMPLIED, &CPU::NOP, "NOP", 4},

	//9
	{MODE_RELATIVE, &CPU::BCC, "BCC", 2},			{MODE_INDIRECTINDEXED, &CPU::STA, "STA", 6},	{MODE_IMPLIED, &CPU::NOP, "NOP", 0},			{MODE_IMPLIED, &CPU::NOP, "NOP", 6},
	{MODE_ZEROPAGEX, &CPU::STY, "STY", 4},		{MODE_ZEROPAGEX, &CPU::STA, "STA", 4},		{MODE_ZEROPAGEY, &CPU::STX, "STX", 4},		{MODE_IMPLIED, &CPU::NOP, "NOP", 4},
	{MODE_IMPLIED, &CPU::TYA, "TYA", 2},			{MODE_ABSOLUTEY, &CPU::STA, "STA", 5},		{MODE_IMPLIED, &CPU::TXS, "TXS", 2},			{MODE_IMPLIED, &CPU::NOP, "NOP", 5},
	{MODE_IMPLIED, &CPU::NOP, "NOP", 5},			{MODE_ABSOLUTEX, &CPU::STA, "STA", 5},		{MODE_IMPLIED, &CPU::NOP, "NOP", 5},			{MODE_IMPLIED, &CPU::NOP, "NOP", 5},

	//A
	{MODE_IMMEDIATE, &CPU::LDY, "LDY", 2},		{MODE_INDEXEDINDIRECT, &CPU::LDA, "LDA", 6},	{MODE_IMMEDIATE, &CPU::LDX, "LDX", 2},		{MODE_IMPLIED, &CPU::NOP, "NOP", 6},
	{MODE_ZEROPAGE, &CPU::LDY, "LDY", 3},			{MODE_ZEROPAGE, &CPU::LDA, "LDA", 3},			{MODE_ZEROPAGE, &CPU::LDX, "LDX", 3},			{MODE_IMPLIED, &CPU::NOP, "NOP", 3},
	{MODE_IMPLIED, &CPU::TAY, "TAY", 2},			{MODE_IMMEDIATE, &CPU::LDA, "LDA", 2},		{MODE_IMPLIED, &CPU::TAX, "TAX", 2},			{MODE_IMPLIED, &CPU::NOP, "NOP", 2},
	{MODE_ABSOLUTE, &CPU::LDY, "LDY", 4},			{MODE_ABSOLUTE, &CPU::LDA, "LDA", 4},			{MODE_ABSOLUTE, &CPU::LDX, "LDX", 4},			{MODE_IMPLIED, &CPU::NOP, "NOP", 4},

	//B
	{MODE_RELATIVE, &CPU::BCS, "BCS", 2},			{MODE_INDIRECTINDEXED, &CPU::LDA, "LDA", 5},	{MODE_IMPLIED, &CPU::NOP, "NOP", 0},			{MODE_IMPLIED, &CPU::NOP, "NOP", 5},
	{MODE_ZEROPAGEX, &CPU::LDY, "LDY", 4},		{MODE_ZEROPAGEX, &CPU::LDA, "LDA", 4},		{MODE_ZEROPAGEY, &CPU::LDX, "LDX", 4},		{MODE_IMPLIED, &CPU::NOP, "NOP", 4},
	{MODE_IMPLIED, &CPU::CLV, "CLV", 2},			{MODE_ABSOLUTEY, &CPU::LDA, "LDA", 4},		{MODE_IMPLIED, &CPU::TSX, "TSX", 2},			{MODE_IMPLIED, &CPU::NOP, "NOP", 4},
	{MODE_ABSOLUTEX, &CPU::LDY, "LDY", 4},		{MODE_ABSOLUTEX, &CPU::LDA, "LDA", 4},		{MODE_ABSOLUTEY, &CPU::LDX, "LDX", 4},		{MODE_IMPLIED, &CPU::NOP, "NOP", 4},

	//C
	{MODE_IMMEDIATE, &CPU::CPY, "CPY", 2},		{MODE_INDEXEDINDIRECT, &CPU::CMP, "CMP", 6},	{MODE_IMPLIED, &CPU::NOP, "NOP", 2},			{MODE_IMPLIED, &CPU::NOP, "NOP", 8},
	{MODE_ZEROPAGE, &CPU::CPY, "CPY", 3},			{MODE_ZEROPAGE, &CPU::CMP, "CMP", 3},			{MODE_ZEROPAGE, &CPU::DEC, "DEC", 5},			{MODE_IMPLIED, &CPU::NOP, "NOP", 5},
	{MODE_IMPLIED, &CPU::INY, "INY", 2},			{MODE_IMMEDIATE, &CPU::CMP, "CMP", 2},		{MODE_IMPLIED, &CPU::DEX, "DEX", 2},			{MODE_IMPLIED, &CPU::NOP, "NOP", 2},
	{MODE_ABSOLUTE, &CPU::CPY, "CPY", 4},			{MODE_ABSOLUTE, &CPU::CMP, "CMP", 4},			{MODE_ABSOLUTE, &CPU::DEC, "DEC", 6},			{MODE_IMPLIED, &CPU::NOP, "NOP", 6},

	//D
	{MODE_RELATIVE, &CPU::BNE, "BNE", 2},			{MODE_INDIRECTINDEXED, &CPU::CMP, "CMP", 5},	{MODE_IMPLIED, &CPU::NOP, "NOP", 0},			{MODE_IMPLIED, &CPU::NOP, "NOP", 8},
	{MODE_IMPLIED, &CPU::NOP, "NOP", 4},			{MODE_ZEROPAGEX, &CPU::CMP, "CMP", 4},		{MODE_ZEROPAGEX, &CPU::DEC, "DEC", 6},		{MODE_IMPLIED, &CPU::NOP, "NOP", 6},
	{MODE_IMPLIED, &CPU::CLD, "CLD", 2},			{MODE_ABSOLUTEY, &CPU::CMP, "CMP", 4},		{MODE_IMPLIED, &CPU::NOP, "NOP", 2},			{MODE_IMPLIED, &CPU::NOP, "NOP", 7},
	{MODE_IMPLIED, &CPU::NOP, "NOP", 4},			{MODE_ABSOLUTEX, &CPU::CMP, "CMP", 4},		{MODE_ABSOLUTEX, &CPU::DEC, "DEC", 7},		{MODE_IMPLIED, &CPU::NOP, "NOP", 7},

	//E
	{MODE_IMMEDIATE, &CPU::CPX, "CPX", 2},		{MODE_INDEXEDINDIRECT, &CPU::SBC, "SBC", 6},	{MODE_IMPLIED, &CPU::NOP, "NOP", 2},			{MODE_IMPLIED, &CPU::NOP, "NOP", 8},
	{MODE_ZEROPAGE, &CPU::CPX, "CPX", 3},			{MODE_ZEROPAGE, &CPU::SBC, "SBC", 3},			{MODE_ZEROPAGE, &CPU::INC, "INC", 5},			{MODE_IMPLIED, &CPU::NOP, "NOP", 5},
	{MODE_IMPLIED, &CPU::INX, "INX", 2},			{MODE_IMMEDIATE, &CPU::SBC, "SBC", 2},		{MODE_IMPLIED, &CPU::NOP, "NOP", 2},			{MODE_IMPLIED, &CPU::NOP, "NOP", 2},
	{MODE_ABSOLUTE, &CPU::CPX, "CPX", 4},			{MODE_ABSOLUTE, &CPU::SBC, "SBC", 4},			{MODE_ABSOLUTE, &CPU::INC, "INC", 6},			{MODE_IMPLIED, &CPU::NOP, "NOP", 6},

	//F
	{MODE_RELATIVE, &CPU::BEQ, "BEQ", 2},			{MODE_INDIRECTINDEXED, &CPU::SBC, "SBC", 5},	{MODE_IMPLIED, &CPU::NOP, "NOP", 0},			{MODE_IMPLIED, &CPU::NOP, "NOP", 8},
	{MODE_IMPLIED, &CPU::NOP, "NOP", 4},			{MODE_ZEROPAGEX, &CPU::SBC, "SBC", 4},		{MODE_ZEROPAGEX, &CPU::INC, "INC", 6},		{MODE_IMPLIED, &CPU::NOP, "NOP", 6},
	{MODE_IMPLIED, &CPU::SED, "SEC", 2},			{MODE_ABSOLUTEY, &CPU::SBC, "SBC", 4},		{MODE_IMPLIED, &CPU::NOP, "NOP", 2},			{MODE_IMPLIED, &CPU::NOP, "NOP", 7},
	{MODE_IMPLIED, &CPU::NOP, "NOP", 4},			{MODE_ABSOLUTEX, &CPU::SBC, "SBC", 4},		{MODE_ABSOLUTEX, &CPU::INC, "INC", 7},		{MODE_IMPLIED, &CPU::NOP, "NOP", 7}
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
	ImGui::SetWindowPos(ImVec2(1920 - 350, 75));

	ImGui::Text("A: %s X: %s Y: %s", Util::hex(A).c_str(), Util::hex(X).c_str(), Util::hex(Y).c_str());
	ImGui::Text("SP: %s PC: %s Opcode: %s", Util::hex(SP).c_str(), Util::hex(PC).c_str(), Util::hex(bus->Read(PC)).c_str());


	unsigned int index = PC;
	for (unsigned int i = 0; i < 10; i++) {
		auto disassembly = DisassembleInstruction(index);

		ImGui::Text("$%s - %s", Util::hex(index).c_str(), disassembly.first.c_str());

		index += disassembly.second;
	}

	
	ImGui::End();

}
std::string CPU::DisassembleROM() {
	return "";
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

std::pair<std::string, int> CPU::DisassembleInstruction(uint16_t memLocation) {

	uint8_t opcode = bus->Read(memLocation);
	std::string instruction = opcodes[opcode].name;
	std::stringstream ss;

	int length = 0;

	uint8_t offset = 0;
	uint16_t val = 0;
	uint16_t high = 0;
	uint16_t low = 0;
	uint16_t high2 = 0;
	uint16_t low2 = 0;
	uint16_t zp = 0;

	switch (opcodes[opcode].addrMode) {
		case MODE_IMPLIED:
			ss << instruction;
			length = 1;
			break;
		case MODE_ACCUMULATOR:
			ss << instruction;
			length = 1;
			break;
		case MODE_IMMEDIATE:
			ss << (instruction) << " #$" << (Util::hex(bus->Read(memLocation + 1)));
			length = 2;
			break;
		case MODE_ZEROPAGE:
			ss << (instruction) << " $" << (Util::hex(bus->Read(memLocation + 1)));
			length = 2;
			break;
		case MODE_ZEROPAGEX:
			ss << (instruction) << " $" << (Util::hex(bus->Read(memLocation + 1))) << ", X";
			length = 2;
			break;
		case MODE_ZEROPAGEY:
			ss << (instruction) << " $" << (Util::hex(bus->Read(memLocation + 1))) << ", Y";
			length = 2;
			break;
		case MODE_RELATIVE:
			offset = bus->Read(memLocation + 1);
			val = 0;
			if (offset > 127) {
				offset = ~offset + 1;
				val = memLocation - offset;
			}
			else {
				val = memLocation + offset;
			}
			ss << instruction << " $" << (Util::hex(memLocation));
			length = 2;
			break;
		case MODE_ABSOLUTE:
			low = bus->Read(memLocation + 1);
			high = bus->Read(memLocation + 2);

			ss << (instruction) << " $" << (Util::hex((high << 8) + low)); 
			length = 3;
			break;
		case MODE_ABSOLUTEX:
			low = bus->Read(memLocation + 1);
			high = bus->Read(memLocation + 2);

			ss << (instruction) << " $" << (Util::hex((high << 8) + low)) << ", X";
			length = 3;

			break;
		case MODE_ABSOLUTEY:
			low = bus->Read(memLocation + 1);
			high = bus->Read(memLocation + 2);

			ss << instruction << " $" << (Util::hex((high << 8) + low)) << ", Y";
			length = 3;
			break;
		case MODE_INDIRECT:
			low = bus->Read(memLocation + 1);
			high = bus->Read(memLocation + 2);

			low2 = bus->Read((high << 8) + low);
			high2 = bus->Read((high << 8) + low + 1);

			ss << instruction << " $" << Util::hex((high2 << 8) + low2) << "";
			length = 3;
			break;
		case MODE_INDEXEDINDIRECT:
			zp = bus->Read(memLocation + 1);
			low = bus->Read(zp);
			high = bus->Read(zp + 1);

			ss << instruction << " ($" << (Util::hex((high << 8) + low)) << ", X)";
			length = 3;
			break;
		case MODE_INDIRECTINDEXED:
			zp = bus->Read(memLocation + 1);

			low = bus->Read(zp);
			high = bus->Read(zp + 1);

			ss << instruction << " ($" << (Util::hex((high << 8) + low)) << "), Y";
			length = 3;

			break;
		default:
			length = 1;
			ss << "INVALID ADDRESSING MODE";
			break;
	}
	return std::pair<std::string, int>(ss.str(), length);
	
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

	//if (PC == 0x8081) {
	//	std::cout << "ILLEGAL MOVE\n";
	//}

	bus->Write(0xFE, (uint8_t)rand());

	opcode = bus->Read(PC);
	
	delay = opcodes[opcode].delay - 1;

	//PrintState();

	address = addrMap[opcodes[opcode].addrMode](*this); // call addressing mode

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
