#include "raylib.h"
#include "chip8.h"
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

void handle_error_code(int error_code);
void update_key_state();

Chip8 chip8;

const int 	DISPLAY_WIDTH  	= 640;
const int 	DISPLAY_HEIGHT 	= 320;
const int 	PIXAL_SIZE 		= 10;
const char* TITLE 			= "chip8 emulator";

const int INPUT_KEYS[16] = {
	KEY_ONE, 	KEY_TWO, 	KEY_THREE, 	KEY_FOUR,
	KEY_Q, 		KEY_W, 		KEY_E, 		KEY_R,
	KEY_A, 		KEY_S, 		KEY_D, 		KEY_F,
	KEY_Z, 		KEY_X, 		KEY_C, 		KEY_V
};

int main(int argc, char *argv[])
{
	time_t time_stamp = time(NULL);
	int error_code = 0, key = 0;
	char charactor = 0;
	std::ostringstream oss;

	InitWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, TITLE);
	SetTargetFPS(60);

	if (argc != 2)
	{
		std::cout << "invalid argument";
		return 0;
	}

	error_code = chip8.load(std::string(argv[1]));

	if(error_code)
	{
		handle_error_code(error_code);
		return 0;
	}

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
		
		// we have to update the screen at 60hz, since system expects atleast 60 frames per second
		// we set target_fps as 60, have to deliver the 60 frames
		BeginDrawing();
		ClearBackground(BLACK);
		DrawText(oss.str().c_str(), 50, 50, 20, WHITE);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}

void update_key_state()
{
	for (int i = 0; i < KEYS; i++)
	{
		chip8.key_state[i] = IsKeyDown(INPUT_KEYS[i]);
	}
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
