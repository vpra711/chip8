#include <fstream>
#include "game.h"

int error_code = 0, key = 0;
char charactor = 0;
std::ostringstream oss;
Chip8 chip8;

int initialize(char *filename)
{
	InitWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, TITLE);
	SetTargetFPS(60);

	std::vector<byte> buffer = load_file_into_buffer(filename);

	if(buffer.empty())
	{
		return 1;
	}

	chip8.load(buffer);

	// if (error_code)
	// {
	// 	handle_error_code(error_code);
	// 	return 0;
	// }
}

void run()
{
	while (!WindowShouldClose())
	{
		update_key_state();

		if (key != 0 || charactor != 0)
		{
			chip8.emulate_one_cycle();
			oss.clear();
			oss.str("");
			oss << "key is: " << key << ", char is: " << charactor;
		}

		update_display();
	}
}

void close()
{
	CloseWindow();
}

std::vector<byte> load_file_into_buffer(char *filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	
	if (!file.is_open())
	{
		return {};
	}
	
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	
	if(size <= 0) 
	{
		return {};
	}
	
	std::vector<byte> buffer(size);
	
	if ((!file.read((char *)&buffer[0], size)) && (file.eof() || file.bad() || file.fail()))
	{
		return {};
	}
	
	return buffer;
}

void update_key_state()
{
	key = GetKeyPressed();
	charactor = GetCharPressed();

	// updating chip8's key status from the mapped keys
	for (int i = 0; i < KEYS; i++)
	{
		chip8.key_state[i] = IsKeyDown(INPUT_KEYS[i]);
	}
}

void update_display()
{
	// we have to update the screen at 60hz, since system expects atleast 60 frames per second
	// we set target_fps as 60, have to deliver the 60 frames
	BeginDrawing();
	ClearBackground(BLACK);
	DrawText(oss.str().c_str(), 50, 50, 20, WHITE);
	EndDrawing();
}

std::ostringstream get_memory_as_str_stream()
{
	// std::ostringstream output_buffer;
	// output_buffer << "\n";

	// for (int i = 0; i < MEMORY_SIZE; i++)
	// {
	// 	if (i % 8 == 0)
	// 	{
	// 		output_buffer << "\n";
	// 	}

	// 	output_buffer << chip8.memory[i] << "\t";
	// }

	// return output_buffer;
}

void handle_error_code(int error_code)
{
	std::string error_message;
	switch (error_code)
	{
		case 1:
		error_message = "unable to open file";
		break;
		case 2:
		error_message = "file is empty";
		break;
		case 3:
		error_message = "unable to read";
		break;
		case 4:
		error_message = "end of file detected";
		break;
		case 5:
		error_message = "bad file";
		break;
		case 6:
		error_message = "fail bit is set";
		break;
		default:
		error_message = "unknown error";
		break;
	}
}
