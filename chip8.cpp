#include <vector>
#include <time.h>
#include "chip8.h"

Chip8::Chip8()
{
	// initializaiton of chip-8
	// game program starts at 0x200 (512)
	pc = 0x200;

	// keyboard configuration
	memcpy(memory, chip8_fontset, FONTSET_SIZE);

	srand(time(0));
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
	word opcode = (memory[pc] << 8) | memory[pc + 1];
	int first_nibble_filter = opcode & 0xF000;

	switch (first_nibble_filter)
	{
		case 0x0000:
			zero_series(opcode);
			break;
		case 0x1000:
			// jump to location nnn (0x1nnn)
			pc = opcode & 0x0FFF;
			break;
		case 0x2000: 
			// call subroutine at nnn (0x2nnn)
			stack[sp] = pc;
			sp++;
			pc = opcode & 0x0FFF;
			break;
		case 0x3000:
			int x  = opcode & 0x0F00;
			int kk = opcode & 0x00FF;
			if (v[x] == kk)
				pc += 2;
			break;
		case 0x4000:
			int x  = opcode & 0x0F00;
			int kk = opcode & 0x00FF;
			if (v[x] != kk)
				pc += 2;
			break;
		case 0x5000:
			int x = opcode & 0x0F00;
			int y = opcode & 0x00F0;
			if (v[x] == v[y])
				pc += 2;
			break;
		case 0x6000:
			int x = opcode & 0x0F00;
			int kk = opcode & 0x00FF;
			v[x] = kk;
			break;
		case 0x7000:
			int x = opcode & 0x0F00;
			int kk = opcode & 0x00FF;
			v[x] += kk;
			break;
		case 0x8000:
			eight_series(opcode);
			break;
		case 0x9000:
			int x = opcode & 0x0F00;
			int y = opcode & 0x00F0;
			if (v[x] != v[y])
				pc += 2;
			break;
		case 0xA000:
			i = opcode & 0x0FFF;
			break;
		case 0xB000: 
			pc = (opcode & 0x0FFF) + v[0];
			break;
		case 0xC000:
			int r = rand() % 256;
			v[x] = r & (opcode & 0x00FF);
			break;
		case 0xD000: break;
		case 0xE000:
			e_series(opcode);
			break;
		case 0xF000:
			f_series(opcode);
			break;
	}
}

void Chip8::zero_series(word opcode)
{
	if (opcode == 0x00E0)
	{
		clear_display();
	} 
	else if (opcode == 0x00EE)
	{
		// return from subroutine
		pc = stack[sp];
		sp--;
	}
	else
	{
		// jump to addr nnn in 0x0nnn
		pc = (opcode & 0x0FFF);
	}
}

void Chip8::eight_series(word opcode)
{
	int filter_last_nibble = opcode & 0x000F;

	switch (filter_last_nibble)
	{
		case 0x8000: break;
		case 0x8001: break;
		case 0x8002: break;
		case 0x8003: break;
		case 0x8004: break;
		case 0x8005: break;
		case 0x8006: break;
		case 0x8007: break;
		case 0x800E: break;
	}
}

void Chip8::e_series(word opcode)
{
	int filter_last_2_nibble = opcode & 0x00FF;

	switch (filter_last_2_nibble)
	{
		case 0xE09E: break;
		case 0xE0A1: break;
	}
}

void Chip8::f_series(word opcode)
{
	int filter_last_2_nibble = opcode & 0x00FF;

	switch (filter_last_2_nibble)
	{
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
}
