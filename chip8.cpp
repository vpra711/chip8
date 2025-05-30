#include <vector>
#include "chip8.h"

Chip8::Chip8()
{
	// initializaiton of chip-8
	// game program starts at 0x200 (512)
	pc = 0x200;

	// keyboard configuration
	memcpy(memory, chip8_fontset, FONTSET_SIZE);
}

void Chip8::load(std::vector<byte> buffer)
{
	// game starts at memory 0x200 (512 in memory)
	std::copy(buffer.begin(), buffer.end(), memory + 0x200);
}

void Chip8::reset()
{
	i           = 0;
	sp          = 0;
	opcode      = 0;
	delay_timer = 0;
	sound_timer = 0;
	pc          = 0x200;
	
	memset(memory,    0, MEMORY_SIZE );
	memset(stack,     0, STACK_SIZE  );
	memset(v,         0, REGISTERS   );
	memset(display,   0, DISPLAY_SIZE);
	memset(key_state, 0, KEYS        );

	memcpy(memory, chip8_fontset, FONTSET_SIZE);
}

void Chip8::clear_display()
{
	memset(display, 0, DISPLAY_SIZE);
}

void Chip8::emulate_one_cycle()
{
	word op_code = (memory[pc] << 8) | memory[pc + 1];
	int first_nibble_filter = op_code & 0xF000;
	int first_last_nibble_filter = op_code & 0xF00F;
	int first_last_2_filter = op_code & 0xF0FF;

	switch (first_nibble_filter)
	{
		case 0x1000: break;
		case 0x2000: break;
		case 0x3000: break;
		case 0x4000: break;
		case 0x5000: break;
		case 0x6000: break;
		case 0x7000: break;
		case 0x8000: break;
		case 0x9000: break;
		case 0xA000: break;
		case 0xB000: break;
		case 0xC000: break;
		case 0xD000: break;
	}

	switch (first_last_nibble_filter) 
	{
		case 0x8001: break;
		case 0x8002: break;
		case 0x8003: break;
		case 0x8004: break;
		case 0x8005: break;
		case 0x8006: break;
		case 0x8007: break;
		case 0x800E: break;
	}

	switch (first_last_2_filter)
	{
		case 0xE09E: break;
		case 0xE0A1: break;
		case 0xF007: break;
		case 0xF00A: break;
		case 0xF015: break;
		case 0xF018: break;
		case 0xF01E: break;
		case 0xF029: break;
		case 0xF033: break;
		case 0xF055: break;
		case 0xF065: break;
	}

	if (op_code == 0x00E0)
	{
		clear_display();
	} 
	else if (op_code == 0x00E0)
	{
		// return from subroutine
		pc = stack[sp];
		sp--;
	}
}
