#pragma once
#include <vector>
#include <cstdlib>
#include <ctime>
#include <memory.h>

typedef unsigned char byte;
typedef unsigned short word;

constexpr int  MEMORY_SIZE 		= 4 * 1024;
constexpr int  DISPLAY_HEIGHT	= 32;
constexpr int  DISPLAY_WIDTH 	= 64;
constexpr int  FONTSET_SIZE 	= 80;
constexpr int  STACK_SIZE 		= 16;
constexpr int  REGISTERS 		= 16;
constexpr int  KEYS 			= 16;
constexpr byte F 				= 0xF;

constexpr byte chip8_fontset[FONTSET_SIZE] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

enum CHIP8_RESULT
{
	copy_memory_buffer_success,
	copy_memory_buffer_error,
};

struct Opcode
{
	word 	data;
	word 	x;
	word 	y;
	word 	kk;
	word 	nnn;

	Opcode();
	Opcode(word instruction);
};

struct Chip8
{
	private:
	// chip8 has 4k of memory (0xFFF)
	byte memory[MEMORY_SIZE];


	// total 16 register, each 8 bits
	// (vx) V0 ... VF, 16th register used as a carry flag
	byte v[REGISTERS];

	// memory address register (index register), 0x0 ... 0xFFF
	word I;

	// program counter, 0x0 ... 0xFFF
	word pc;

	// opcode, usually 2 bytes, but will expand, 
	// word opcode;
	Opcode opcode;

	// system call stack
	// 16 level - each 16 bit
	word stack[STACK_SIZE];
	word sp;


	public:
	// 2 special purpose registers - delay and sound timers	
	// delay timer 8 bit
	byte delay_timer;

	// sound timer 8 bit
	byte sound_timer;

	// windows size 64 * 32 => 2048
	byte display[DISPLAY_WIDTH][DISPLAY_HEIGHT];

	// keyboard - 0x0 ... 0xF
	byte key_state[KEYS];

	bool waiting_for_key_press;


	private:
	void clear_display();

	void zero_series();

	void eight_series();

	void e_series();

	void f_series();

	void update_diplay_memory();


	public : 
	Chip8();

	CHIP8_RESULT load(std::vector<byte> buffer);

	void emulate_one_cycle();

	void check_keys();

	void set_key(byte key);

	void reset();
};
