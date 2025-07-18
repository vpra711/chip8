#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <fstream>
#include <raylib.h>
#include "chip8.h"

typedef std::chrono::time_point<std::chrono::high_resolution_clock> chiptime;

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr int PIXAL_SIZE = 10;
constexpr char *TITLE = "chip8 emulator";
constexpr char *sound_file = "tests/beep.mp3";
constexpr float TIME_INTERVAL = 1.0 / 60.0; // 60hz

constexpr int INPUT_KEYS[16] =
{
	KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR,
	KEY_Q, KEY_W, KEY_E, KEY_R,
	KEY_A, KEY_S, KEY_D, KEY_F,
	KEY_Z, KEY_X, KEY_C, KEY_V
};

enum Error
{
	file_not_found,
	file_empty,
	unable_to_read,
	end_of_file,
	bad_file,
	fail_bit_is_sit,
	buffer_empty,
	unknown,
	none
};

// main functions
Error initialize(char *filename);
void run();
void update_display();
void close();

// helper functions
std::vector<byte> load_file_into_buffer(char *filename);
void update_key_state();
void update_execution();
void update_display();
void update_timers();
int get_key_presss();
