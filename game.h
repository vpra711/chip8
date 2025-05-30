#pragma once
#include <string>
#include <sstream>
#include "raylib.h"
#include "chip8.h"

constexpr int DISPLAY_WIDTH = 640;
constexpr int DISPLAY_HEIGHT = 320;
constexpr int PIXAL_SIZE = 10;
constexpr char *TITLE = "chip8 emulator";

constexpr int INPUT_KEYS[16] =
{
	KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR,
	KEY_Q, KEY_W, KEY_E, KEY_R,
	KEY_A, KEY_S, KEY_D, KEY_F,
	KEY_Z, KEY_X, KEY_C, KEY_V
};

// main functions
int initialize(char *filename);
void run();
void update_display();
void close();

// helper functions
std::vector<byte> load_file_into_buffer(char *filename);
void update_key_state();
std::ostringstream get_memory_as_str_stream();
void handle_error_code(int error_code);
