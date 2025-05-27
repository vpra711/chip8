#pragma once
#include <string>
#include <sstream>

const int MEMORY_SIZE   = 4  * 1024;
const int DISPLAY_SIZE  = 64 * 32;
const int REGISTERS     = 16;
const int STACK_SIZE    = 16;
const int KEYS          = 16;

struct Chip8
{
	// chip8 has 4k of memory (0xFFF)
	unsigned char memory[MEMORY_SIZE];


	// total 16 register, each 8 bits
	// (vx) V0 ... VF, 16th register used as a carry flag
	unsigned char v[REGISTERS];

	// memory address register (index register), 0x0 ... 0xFFF
	unsigned short i;
	
	// program counter, 0x0 ... 0xFFF
	unsigned short pc;


	// 2 special purpose registers - delay and sound timers	
	// delay timer 8 bit
	unsigned char delay_timer;

	// sound timer 8 bit
	unsigned char sound_timer;


	// opcode, 2 bytes
	unsigned short opcode;

	// windows size 64 * 32 => 2048
	unsigned char display[DISPLAY_SIZE];


	// system call stack
	// 16 level - each 16 bit
	unsigned short stack[STACK_SIZE];
	unsigned short sp;

	// keyboard - 0x0 ... 0xF
	unsigned char key_state[KEYS];


	Chip8();

	int load(std::string file_name);

	void emulate_one_cycle();

	void check_keys();

	void reset();

	std::ostringstream get_memory_as_str_stream();
};
