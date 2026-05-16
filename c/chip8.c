#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "../raylib/include/raylib.h"

#define MEMORY_SIZE (4 * 1024)
#define DATA_REGISTER_SIZE 16
#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH 64
#define STACK_SIZE 16
#define F 0xF
#define PIXEL_SIZE 10

typedef uint8_t byte;
typedef uint16_t word;
typedef void (*handler)(word opcode);

const char* PROGRAM_NAME = "CHIP-8 VM";
const int DISPLAY_HEIGHT = 600;
const int DISPLAY_WIDTH  = 800;
const int KEYMAP_SIZE    = 16;
const int FONTSET_SIZE   = 5 * 16;

// data registers
byte regter[DATA_REGISTER_SIZE];

//address register or index
word addr;

word program_counter = 0x200;
byte memory[MEMORY_SIZE];
byte screen[SCREEN_WIDTH][SCREEN_HEIGHT];

byte delay_timer;
byte sound_timer;


word stack[STACK_SIZE];
byte stack_pointer;

word data_x, data_y, data_n, data_nn, data_nnn;

const byte chip8_fontset[] = {
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

const int keymap[] = {
    KEY_X,     // 0
    KEY_ONE,   // 1
    KEY_TWO,   // 2
    KEY_THREE, // 3
    KEY_Q,     // 4
    KEY_W,     // 5
    KEY_E,     // 6
    KEY_A,     // 7
    KEY_S,     // 8
    KEY_D,     // 9
    KEY_Z,     // A
    KEY_C,     // B
    KEY_FOUR,  // C
    KEY_R,     // D
    KEY_F,     // E
    KEY_V,     // F
};

void operation_0xxx(word opcode) {
    if (opcode == 0x00E0) {
        memset(screen, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
    } else if (opcode == 0x00EE) {
        program_counter = stack[--stack_pointer];
    } else {
        stack[stack_pointer++] = program_counter;
        program_counter = data_nnn;
        // will increase the program counter after dispatch.
        program_counter -= 2;
    }
}

void operation_1xxx(word opcode) {
    program_counter = data_nnn;
    // will increase the program counter after dispatch.
    program_counter -= 2;
}

void operation_2xxx(word opcode) {
    stack[stack_pointer++] = program_counter;
    program_counter = data_nnn;
    // will increase the program counter after dispatch.
    program_counter -= 2;
}

void operation_3xxx(word opcode) {
    if (regter[data_x] == data_nn) {
        program_counter += 2;
    }
}

void operation_4xxx(word opcode) {
    if (regter[data_x] != data_nn) {
        program_counter += 2;
    }
}

void operation_5xxx(word opcode) {
    if (regter[data_x] == regter[data_y]) {
        program_counter += 2;
    }
}

void operation_6xxx(word opcode) {
    regter[data_x] = data_nn;
}

void operation_7xxx(word opcode) {
    regter[data_x] += data_nn;
}

void operation_8xxx(word opcode) {
    switch (data_n) {
        case 0:
            regter[data_x] = regter[data_y];
            break;
        case 1:
            regter[data_x] |= regter[data_y];
            break;
        case 2:
            regter[data_x] &= regter[data_y];
            break;
        case 3:
            regter[data_x] ^= regter[data_y];
            break;
        case 4:
            regter[F] = (regter[data_x] + regter[data_y]) > 255;
            regter[data_x] += regter[data_y];
            break;
        case 5:
            regter[F] = regter[data_x] >= regter[data_y];
            regter[data_x] -= regter[data_y];
            break;
        case 6:
            regter[F] = regter[data_x] & 1;
            regter[data_x] >>= 1;
            break;
        case 7:
            regter[F] = regter[data_y] >= regter[data_x];
            regter[data_x] = regter[data_y] - regter[data_x];
            break;
        case 0xE:
            regter[F] = (regter[data_x] >> 7) & 1;
            regter[data_x] <<= 1;
            break;
        default:
            fprintf(stdout, "invalid operation 8-series. opcode: %d, operation: %d\n", opcode, data_n);
    }
}

void operation_9xxx(word opcode) {
    if (regter[data_x] != regter[data_y]) {
        program_counter += 2;
    }
}

void operation_axxx(word opcode) {
    addr = data_nnn;
}

void operation_bxxx(word opcode) {
    program_counter = data_nnn + regter[0];
    program_counter -= 2;
}

void operation_cxxx(word opcode) {
    regter[data_x] = (rand() % 256) & data_nn;
}

void operation_dxxx(word opcode) {
	regter[F] = 0;

	// mi - memory index
	for (byte mi = 0; mi < data_n; mi++)
	{
		// n bytes from address in memory
		byte memory_byte = memory[addr + mi];

		for (byte bi = 0; bi < 8; bi++)
		{
			byte bit = (memory_byte >> (7 - bi)) & 0x1;
			byte x = (regter[data_x] + bi) % 64;
			byte y = (regter[data_y] + mi) % 32;
			screen[x][y] ^= bit;

			if (bit == 1 && screen[x][y] == 0)
			{
				regter[F] = 1;
			}
		}
	}
}

void operation_exxx(word opcode) {
    word operation = opcode & 0xF0FF;
    if (operation == 0xE09E) {
        if(IsKeyDown(keymap[regter[data_x]])) {
            program_counter += 2;
        }
    } else if (operation == 0xE0A1) {
        if (IsKeyUp(keymap[regter[data_x]])) {
            program_counter += 2;
        }
    } else {
        fprintf(stdout, "invalid operation e-series. opcode: %d, operation: %d\n", opcode, operation);
    }
}

void operation_fxxx(word opcode) {
    switch (data_nn) {
        case 0x07:
            regter[data_x] = delay_timer;
            break;
        case 0x0A:
            while(true) {
                for (int i = 0; i < KEYMAP_SIZE; i++) {
                    if (!IsKeyDown(keymap[i]))
                        continue;
                    regter[data_x] = i;
                    return;
                }
            }
            break;
        case 0x15:
            delay_timer = regter[data_x];
            break;
        case 0x18:
            sound_timer = regter[data_x];
            break;
        case 0x1E:
            addr += regter[data_x];
            break;
        case 0x29:
            // each character is 5 bytes long.
            // fonts are stored in memory location
            // 0x000 to 0x01FF
            addr = regter[data_x] * 5;
            break;
        case 0x33:
            memory[addr] = regter[data_x] / 100;
            memory[addr + 1] = (regter[data_x] / 10) % 10;
            memory[addr + 2] = regter[data_x] % 10;
            break;
        case 0x55:
            memcpy(memory + addr, regter, data_x + 1);
            break;
        case 0x65:
            memcpy(regter, memory + addr, data_x + 1);
            break;
        default:
            fprintf(stdout, "invalid operation f-series. opcode: %d, operation: %d\n", opcode, data_nn);
    }
}

handler dispatch_table[] = {
    operation_0xxx,
    operation_1xxx,
    operation_2xxx,
    operation_3xxx,
    operation_4xxx,
    operation_5xxx,
    operation_6xxx,
    operation_7xxx,
    operation_8xxx,
    operation_9xxx,
    operation_axxx,
    operation_bxxx,
    operation_cxxx,
    operation_dxxx,
    operation_exxx,
    operation_fxxx
};

void emulate_one_cycle() {
    word opcode = (memory[program_counter] << 8) | memory[program_counter + 1];
    data_x      = (opcode & 0x0F00) >> 8;
    data_y      = (opcode & 0x00F0) >> 4;
    data_n      = opcode & 0x000F;
    data_nn     = opcode & 0x00FF;
    data_nnn    = opcode & 0x0FFF;

    int operation = (opcode & 0xF000) >> 12;
    dispatch_table[operation](opcode);

    if (delay_timer > 0) {
        delay_timer--;
    }

    if (sound_timer > 0) {
        sound_timer--;
    }

    program_counter += 2;
}

void update_display() {
    for (byte x = 0; x < SCREEN_WIDTH; x++)
    {
    	for (byte y = 0; y < SCREEN_HEIGHT; y++)
    	{
    		if (screen[x][y] == 1)
    		{
    			DrawRectangle(x * PIXEL_SIZE + PIXEL_SIZE, y * PIXEL_SIZE + PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE, BROWN);
    		}
    		else
    		{
    			DrawRectangle(x * PIXEL_SIZE + PIXEL_SIZE, y * PIXEL_SIZE + PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE, GRAY);
    		}
    	}
    }
}

int main(int argc, char **argv)
{
    srand(time(0));
    FILE *game = fopen("../tests/1-chip8-logo.ch8", "rb");
    if (!game) {
        fprintf(stdout, "failed to open ROM\n");
        return 1;
    }
    fseek(game, 0, SEEK_END);
    int file_size = ftell(game);
    fseek(game, 0, SEEK_SET);
    memcpy(memory, chip8_fontset, FONTSET_SIZE);
    fread(memory + program_counter, 1, file_size, game);
    InitWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, PROGRAM_NAME);
    SetTargetFPS(60);
    while(!WindowShouldClose()) {
        emulate_one_cycle();
        BeginDrawing();
        ClearBackground(BLACK);
        update_display();
        EndDrawing();
        usleep(15000);
    }
    fclose(game);
    return 0;
}
