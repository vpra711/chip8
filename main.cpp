#include "raylib.h"
#include "chip8.h"
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

void handle_error_code(int error_code);

Chip8 chip8;

const int 	DISPLAY_WIDTH  	= 640;
const int 	DISPLAY_HEIGHT 	= 320;
const int 	PIXAL_SIZE 		= 10;
const char* TITLE 			= "chip8 emulator";

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
		key = GetKeyPressed();
		charactor = GetCharPressed();

		if (key != 0 || charactor != 0)
		{
			chip8.emulate_one_cycle();
			oss.clear();
			oss.str("");
			oss << "key is: " << key << ", char is: " << charactor;
		}

		BeginDrawing();
		ClearBackground(BLACK);
		DrawText(oss.str().c_str(), 50, 50, 20, WHITE);
		EndDrawing();
	}

	CloseWindow();
	return 0;
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
