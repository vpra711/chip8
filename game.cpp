#include <chrono>
#include <fstream>
#include <raylib.h>
#include "game.h"

int error_code = 0;
Chip8 chip8;
Sound beep;
chiptime last_time;

int initialize(char *filename)
{
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, TITLE);
	InitAudioDevice();
	SetTargetFPS(60);

	std::vector<byte> buffer = load_file_into_buffer(filename);

	if(buffer.empty())
	{
		return 1;
	}

	beep = LoadSound(sound_file);
	chip8.load(buffer);

	// if (error_code)
	// {
	// 	handle_error_code(error_code);
	// 	return 0;
	// }
	last_time = std::chrono::high_resolution_clock::now();
}

void run()
{
	while (!WindowShouldClose())
	{
		update_timers();
		update_key_state();
		update_execution();
		update_display();
	}
}

void close()
{
	UnloadSound(beep);
	CloseAudioDevice();
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
	// updating chip8's key status from the mapped keys
	for (int i = 0; i < KEYS; i++)
	{
		chip8.key_state[i] = IsKeyDown(INPUT_KEYS[i]);
	}
}

void update_execution()
{
	chip8.emulate_one_cycle();
	if (chip8.waiting_for_key_press)
	{
		chip8.set_key(get_key_presss());
	}
}

void update_display()
{
	// we have to update the screen at 60hz, since system expects atleast 60 frames per second
	// we set target_fps as 60, have to deliver the 60 frames
	BeginDrawing();
	ClearBackground(BLACK);
	for (byte x = 0; x < DISPLAY_WIDTH; x++) 
	{
		for (byte y = 0; y < DISPLAY_HEIGHT; y++)
		{
			if (chip8.display[x][y] == 1)
			{
				DrawRectangle(x * PIXAL_SIZE + PIXAL_SIZE, y * PIXAL_SIZE + PIXAL_SIZE, PIXAL_SIZE, PIXAL_SIZE, BROWN);
			}
			else
			{
				DrawRectangle(x * PIXAL_SIZE + PIXAL_SIZE, y * PIXAL_SIZE + PIXAL_SIZE, PIXAL_SIZE, PIXAL_SIZE, GRAY);
			}
		}
	}
	EndDrawing();

	if (!IsSoundPlaying(beep) && chip8.sound_timer > 0)
	{
		PlaySound(beep);
	}
}

void update_timers()
{
	chiptime end_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end_time - last_time;

	if (elapsed.count() >= TIME_INTERVAL) {
		if (chip8.delay_timer > 0) chip8.delay_timer--;
		if (chip8.sound_timer > 0) chip8.sound_timer--;
		last_time = end_time;
	}
}

int get_key_presss()
{
	int key = 0;
	while(true)
	{
		key = GetKeyPressed();

		for (word i = 0; i < KEYS; i++)
		{
			if (key == INPUT_KEYS[i])
			{
				return key;
			}
		}
	}
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
