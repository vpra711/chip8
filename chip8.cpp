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
	opcode = Opcode((memory[pc] << 8) | memory[pc + 1]);
	word first_nibble_filter = opcode.data & 0xF000;

	switch (first_nibble_filter)
	{
		case 0x0000: 
			zero_series();
			break;
		case 0x1000:
			// jump to location nnn (0x1nnn)
			pc = opcode.nnn;
			break;
		case 0x2000: 
			// call subroutine at nnn (0x2nnn)
			stack[sp] = pc;
			sp++;
			pc = opcode.nnn;
			break;
		case 0x3000:
			if (v[opcode.x] == opcode.kk)
				pc += 2;
			break;
		case 0x4000:
			if (v[opcode.x] != opcode.kk)
				pc += 2;
			break;
		case 0x5000:
			if (v[opcode.x] == v[opcode.y])
				pc += 2;
			break;
		case 0x6000:
			v[opcode.x] = opcode.kk;
			break;
		case 0x7000:
			v[opcode.x] += opcode.kk;
			break;
		case 0x8000:
			eight_series();
			break;
		case 0x9000:
			if (v[opcode.x] != v[opcode.y])
				pc += 2;
			break;
		case 0xA000:
			i = opcode.nnn;
			break;
		case 0xB000: 
			pc = opcode.nnn + v[0];
			break;
		case 0xC000:
			v[opcode.x] = (rand() % 256) & opcode.kk;
			break;
		case 0xD000: break;
		case 0xE000:
			e_series();
			break;
		case 0xF000:
			f_series();
			break;
	}
}

void Chip8::zero_series()
{
	if (opcode.data == 0x00E0)
	{
		clear_display();
	} 
	else if (opcode.data == 0x00EE)
	{
		// return from subroutine
		pc = stack[sp];
		sp--;
	}
}

void Chip8::eight_series()
{
	word filter_last_nibble = opcode.data & 0xF00F;
	word f = 0xF;

	switch (filter_last_nibble)
	{
		case 0x8000:
			v[opcode.x] = v[opcode.y];
			break;
		case 0x8001:
			v[opcode.x] |= v[opcode.y];
			break;
		case 0x8002:
			v[opcode.x] &= v[opcode.y];
			break;
		case 0x8003:
			v[opcode.x] ^= v[opcode.y];
			break;
		case 0x8004: 
			v[f] = ((v[opcode.x] + v[opcode.y]) > 255);
			v[opcode.x] += v[opcode.y];
			break;
		case 0x8005:
			v[f] = (v[opcode.x] > v[opcode.y]);
			v[opcode.x] -= v[opcode.y];
			break;
		case 0x8006:
			v[f] =  (v[opcode.x] & 0x1); // checking for lsb bit
			v[opcode.x] >>= 1;
			break;
		case 0x8007:
			v[f] = (v[opcode.y] > v[opcode.x]);
			v[opcode.x] = v[opcode.y] - v[opcode.x];
			break;
		case 0x800E:
			v[f] = (v[opcode.x] & 0x80); // checking for msb bit
			v[opcode.x] <<= 1;
			break;
	}
}

void Chip8::e_series()
{
	switch (opcode.kk)
	{
		case 0xE09E: 
			if(key_state[v[opcode.x]] == 1)
				pc += 2;
			break;
		case 0xE0A1: 
			if(key_state[v[opcode.x]] == 0)
				pc += 2;
			break;
	}
}

void Chip8::f_series()
{
	switch (opcode.kk)
	{
		case 0xF007:
			v[opcode.x] = delay_timer;
			break;
		case 0xF00A:
			waiting_for_key_press = true;
			break;
		case 0xF015:
			delay_timer = v[opcode.x];
			break;
		case 0xF018:
			sound_timer = v[opcode.x];
			break;
		case 0xF01E:
			i += v[opcode.x];
			break;
		case 0xF029:
			i = v[opcode.x] * 5;
			break;
		case 0xF033:
			memory[i] = v[opcode.x] / 100;
			memory[i + 1] = (v[opcode.x] / 10) % 10;
			memory[i + 2] = v[opcode.x] % 10;
			break;
		case 0xF055:
			memcpy(memory + i, v, opcode.x);
			break;
		case 0xF065:
			memcpy(v, memory + i, opcode.x);
			break;
	}
}

void Chip8::set_key(byte key)
{
	v[opcode.x] = key;
	waiting_for_key_press = false;
}

Opcode::Opcode()
{
	data = 0;
	x 	 = 0;
	y 	 = 0;
	kk 	 = 0;
	nnn  = 0;
}

Opcode::Opcode(word instruction)
{
	data = instruction;
	x 	 = (data & 0x0F00) >> 8;
	y	 = (data & 0x00F0) >> 4;
	kk 	 = data & 0x00FF;
	nnn  = nnn;
}
