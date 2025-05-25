#include <string>
#include "chip8.h"
#include "chip8_font.h"

Chip8::Chip8()
{
	// initializaiton of chip-8
	// game program starts at 0x200
	pc = 0x200;

	for (int i = 0; i < 80; i++)
	{
		memory[i] = chip8_fontset[i];
	}
}

void Chip8::load(std::string filename)
{
	
}

void Chip8::emulate_one_cycle()
{
	
}

void Chip8::reset()
{
	i           = 0;
	sp          = 0;
	opcode      = 0;
	delay_timer = 0;
	sound_timer = 0;
	
	memset(memory,    0, MEMORY_SIZE );
	memset(stack,     0, STACK_SIZE  );
	memset(v,         0, REGISTERS   );
	memset(display,   0, DISPLAY_SIZE);
	memset(key_state, 0, KEYS        );
	
	for (int i = 0; i < 80; i++)
	{
		memory[i] = chip8_fontset[i];
	}
	
	pc          = 0x200;
}

void Chip8::check_keys()
{
	
}
