#pragma once

#include <stdint.h>
#include <functional>

class Bus;

enum flags {
	FLAG_CARRY = 1,
	FLAG_ZERO = 2,
	FLAG_IRQ = 4,
	FLAG_DECIMAL = 8,
	FLAG_BRK = 16,
	FLAG_UNUSED = 32,
	FLAG_OVERFLOW = 64,
	FLAG_NEGATIVE = 128
};

enum addr_mode {
	MODE_IMPLIED,
	MODE_ACCUMULATOR,
	MODE_IMMEDIATE,
	MODE_ZEROPAGE,
	MODE_ZEROPAGEX,
	MODE_ZEROPAGEY,
	MODE_RELATIVE,
	MODE_ABSOLUTE,
	MODE_ABSOLUTEX,
	MODE_ABSOLUTEY,
	MODE_INDIRECT,
	MODE_INDEXEDINDIRECT,
	MODE_INDIRECTINDEXED
};

class CPU;

typedef struct Oprand {
	std::function<uint16_t(CPU&)> addrMode;
	std::function<void(CPU&)> instruction;
	uint8_t delay;
} Oprand;

class CPU
{
public:
	CPU(Bus* b);
	~CPU();


	//Main Loop
	bool Cycle();

	void RestartClear();

	void Reset();
	void IRQ();
	void NMI();

	void PrintState();
	void DrawState();
	void DrawScreen();

	void SetButton(uint8_t key);

	void SetFlag(uint8_t flag, bool val);

	//Addressing modes
	uint16_t AddrImplied();
	uint16_t AddrAccumulator();
	uint16_t AddrImmediate();
	uint16_t AddrZeroPage();
	uint16_t AddrZeroPageX();
	uint16_t AddrZeroPageY();
	uint16_t AddrRelative();
	uint16_t AddrAbsolute();
	uint16_t AddrAbsoluteX();
	uint16_t AddrAbsoluteY();
	uint16_t AddrIndirect();
	uint16_t AddrIndexedIndirect();
	uint16_t AddrIndirectIndexed();

	//Instructions
	void ADC();
	void AND();
	void ASL();
	void BCC();
	void BCS();
	void BEQ();
	void BIT();
	void BMI();
	void BNE();
	void BPL();
	void BRK();
	void BVC();
	void BVS();
	void CLC();
	void CLD();
	void CLI();
	void CLV();
	void CMP();
	void CPX();
	void CPY();
	void DEC();
	void DEX();
	void DEY();
	void EOR();
	void INC();
	void INX();
	void INY();
	void JMP();
	void JSR();
	void LDA();
	void LDX();
	void LDY();
	void LSR();
	void NOP();
	void ORA();
	void PHA();
	void PHP();
	void PLA();
	void PLP();
	void ROL();
	void ROR();
	void RTI();
	void RTS();
	void SBC();
	void SEC();
	void SED();
	void SEI();
	void STA();
	void STX();
	void STY();
	void TAX();
	void TAY();
	void TSX();
	void TXA();
	void TXS();
	void TYA();

private:
	uint8_t A;
	uint8_t X;
	uint8_t Y;
	uint16_t PC;
	uint8_t SP;
	uint8_t FLAGS;
	
	uint8_t opcode;
	uint8_t fetched;
	uint16_t address;
	uint8_t addrMode;
	

	uint8_t delay;

	Bus* bus;
	int cycles;
};

