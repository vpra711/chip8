#include <vector>
#include <fstream>
#include "chip8.h"
#include "chip8_font.h"

Chip8::Chip8()
{
	// initializaiton of chip-8
	// game program starts at 0x200
	pc = 0x200;

	// keyboard configuration
	for (int i = 0; i < 80; i++)
	{
		memory[i] = chip8_fontset[i];
	}
}

int Chip8::load(std::string filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (!file.is_open())
	{
		return 1;
	}

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	if (size <= 0)
	{
		return 2;
	}

	std::vector<char> buffer(size);

	if (!file.read(&buffer[0], size))
	{
		if(file.eof())
		{
			return 4;
		}
		else if (file.bad())
		{
			return 5;
		}
		else if (file.fail())
		{
			return 6;
		}
		return 3;
	}

	// game starts at memory 0x200
	for (int i = 0; i < size; i++)
	{
		memory[i + 512] = buffer[i];
	}

	file.close();
	return 0;
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
	pc          = 0x200;
	
	memset(memory,    0, MEMORY_SIZE );
	memset(stack,     0, STACK_SIZE  );
	memset(v,         0, REGISTERS   );
	memset(display,   0, DISPLAY_SIZE);
	memset(key_state, 0, KEYS        );
	
	for (int i = 0; i < 80; i++)
	{
		memory[i] = chip8_fontset[i];
	}
}

void Chip8::check_keys()
{
	
}

std::ostringstream Chip8::get_memory_as_str_stream()
{
	std::ostringstream output_buffer;
	output_buffer << "\n";

	for (int i = 0; i < MEMORY_SIZE; i++)
	{
		if (i % 8 == 0) 
		{
			output_buffer << "\n";
		}

		output_buffer << memory[i] << "\t";
	}

	return output_buffer;
}
