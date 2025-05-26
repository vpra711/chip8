#include "raylib.h"
#include "chip8.h"
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

Chip8 chip8;

const int DISPLAY_WIDTH  = 640;
const int DISPLAY_HEIGHT = 320;
const int PIXAL_SIZE = 10;
const char* TITLE = "chip8 emulator";

int main(int argc, char *argv[])
{
	time_t time_stamp = time(NULL);
	
	if (argc != 2)
	{
		std::cout << "invalid argument";
		return 0;
	}
	
	if(chip8.load(std::string(argv[1])))
	{
		std::cout << "unable to load game into memory";
		return 0;
	}
	
	InitWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, TITLE);
	SetTargetFPS(60);
	
	while(!WindowShouldClose())
	{
		int key = GetKeyPressed();
		char charactor = GetCharPressed();

		if (key != 0 && charactor != 0)
		{
			chip8.emulate_one_cycle();
			std::ostringstream oss;
			oss << "key is: " << key << ", char is: " << charactor;
			BeginDrawing();
			ClearBackground(BLACK);
			DrawText(oss.str().c_str(), 50, 50, 20, WHITE);
			EndDrawing();
		}
	}

	CloseWindow();	
	return 0;
}
