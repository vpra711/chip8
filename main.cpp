#include "raylib.h"
#include "chip8.h"
#include <cstdio>

Chip8 chip8;

const int DISPLAY_WIDTH  = 640;
const int DISPLAY_HEIGHT = 320;
const int PIXAL_SIZE = 10;
const char* TITLE = "chip8 emulator";

int main()
{
	InitWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, TITLE);
	SetTargetFPS(60);

	while(!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);
		chip8.emulate_one_cycle();

		DrawText("hi", 50, 50, 20, WHITE);

		EndDrawing();
	}

	CloseWindow();	
	return 0;
}
