#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "../raylib/include/raylib.h"

#define MEMORY_SIZE (4 * 1024)
#define DATA_REGISTER_SIZE 16
#define SCREEN_SIZE (64 * 32)
#define STACK_SIZE 16
#define F 0xF
#define FONTSET_SIZE (5 * 16)

typedef uint8_t byte;
typedef uint16_t word;
typedef void (*handler)(word opcode);

const char* PROGRAM_NAME = "CHIP-8 VM";
const int DISPLAY_HEIGHT = 600;
const int DISPLAY_WIDTH = 800;

byte memory[MEMORY_SIZE];
// data registers
byte v[DATA_REGISTER_SIZE];
//address register or index
word i;
word pc;

byte delay_timer;
byte sound_timer;

byte screen[SCREEN_SIZE];

word stack[STACK_SIZE];
byte sp;

byte x, y, n, nn, nnn;

const byte chip8_fontset[FONTSET_SIZE] =
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

void draw(int x, int y, int n) {

}

void operation_0xxx(word opcode) {
    if (opcode == 0x00E0) {
        memset(screen, 0, SCREEN_SIZE);
    } else if (opcode == 0x00EE) {
        pc = stack[sp--];
    } else {
        stack[sp++] = pc;
        pc = memory[opcode & 0x0FFF];
    }
}

void operation_1xxx(word opcode) {
    pc = memory[opcode & 0x0FFF];
}

void operation_2xxx(word opcode) {
    stack[sp++] = pc;
    pc = memory[opcode & 0x0FFF];
}

void operation_3xxx(word opcode) {
    if (v[x] == nn) {
        pc += 2;
    }
}

void operation_4xxx(word opcode) {
    if (v[x] != nn) {
        pc += 2;
    }
}

void operation_5xxx(word opcode) {
    if (v[x] == v[y]) {
        pc += 2;
    }
}

void operation_6xxx(word opcode) {
    v[x] = nn;
}

void operation_7xxx(word opcode) {
    v[x] += nn;
}

void operation_8xxx(word opcode) {
    switch (n) {
        case 0:
            v[x] = v[y];
            break;
        case 1:
            v[x] = v[x] | v[y];
            break;
        case 2:
            v[x] = v[x] & v[y];
            break;
        case 3:
            v[x] = v[x] ^ v[y];
            break;
        case 4:
            v[F] = (v[x] + v[y]) > 255;
            v[x] += v[y];
            break;
        case 5:
            v[F] = v[x] >= v[y];
            v[x] -= v[y];
            break;
        case 6:
            v[F] = v[x] & 1;
            v[x] >>= 1;
            break;
        case 7:
            v[F] = v[y] >= v[x];
            v[x] = v[y] - v[x];
            break;
        case 0xE:
            v[F] = v[x] & 0x80;
            v[x] <<= 1;
            break;
        default:
            fprintf(stdout, "invalid operation 8-series. opcode: %d, operation: %d", opcode, n);
    }
}

void operation_9xxx(word opcode) {
    if (v[x] != v[y]) {
        pc += 2;
    }
}

void operation_axxx(word opcode) {
    i = opcode & 0x0FFF;
}

void operation_bxxx(word opcode) {
    pc = (opcode & 0x0FFF) + v[0];
}

void operation_cxxx(word opcode) {
    v[x] = rand() & nn;
}

void operation_dxxx(word opcode) {
    draw(v[x], v[y], n);
}

void operation_exxx(word opcode) {
    word operation = opcode & 0xF0FF;
    if (operation == 0xE09E) {
        if(IsKeyDown(v[x])) {
            pc += 2;
        }
    } else if (operation == 0xE0A1) {
        if (IsKeyUp(v[x])) {
            pc += 2;
        }
    } else {
        fprintf(stdout, "invalid operation e-series. opcode: %d, operation: %d", opcode, operation);
    }
}

void operation_fxxx(word opcode) {
    switch (nn) {
        case 0x07:
            v[x] = delay_timer;
            break;
        case 0x0A:
            while((v[x] = GetKeyPressed()) == 0);
            break;
        case 0x15:
            delay_timer = v[x];
            break;
        case 0x18:
            sound_timer = v[x];
            break;
        case 0x1E:
            i += v[x];
            break;
        case 0x29:
            // each character is 5 bytes long.
            // fonts are stored in memory location
            // 0x000 to 0x01FF
            i = v[x] * 5;
            break;
        case 0x33:
            memory[i] = v[x] / 100;
            memory[i + 1] = (v[x] / 10) % 10;
            memory[i + 2] = v[x] % 10;
            break;
        case 0x55:
            memcpy(memory + i, v, x + 1);
            break;
        case 0x65:
            memcpy(v, memory + i, x + 1);
            break;
        default:
            fprintf(stdout, "invalid operation f-series. opcode: %d, operation: %d", opcode, nn);
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
    operation_fxxx,
};

void emulate_one_cycle() {
    word opcode = (memory[pc] << 8) | memory[pc + 1];
    x = (opcode & 0x0F00) >> 8;
    y = (opcode & 0x00F0) >> 4;
    n = opcode & 0x000F;
    nn = opcode & 0x00FF;
    nnn = opcode & 0x0FFF;

    int operation = (opcode & 0xF000) >> 12;
    dispatch_table[operation](opcode);

    delay_timer--;
    if (delay_timer < 0) {
        delay_timer = 0;
    }

    sound_timer--;
    if (sound_timer < 0) {
        sound_timer = 0;
    }
}

int main(int argc, char **argv)
{
    srand(time(0));
    FILE *game = fopen("../tests/2-ibm-logo.ch8", "r");
    fseek(game, 0, SEEK_END);
    int file_size = ftell(game);
    fseek(game, 0, SEEK_SET);
    memcpy(memory + 0x200, game, file_size);
    memcpy(memory, chip8_fontset, FONTSET_SIZE);
    InitWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, PROGRAM_NAME);
    SetTargetFPS(60);
    while(!WindowShouldClose()) {
        emulate_one_cycle();
        usleep(16000);
        BeginDrawing();
        ClearBackground((Color){});
        EndDrawing();
    }
    return 0;
}
