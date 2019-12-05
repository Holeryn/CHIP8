#ifndef CHIP8
#define CHIP8

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "include/gfx.h"

#define SELECTED_ROM argv[1]

#define bool int
#define true    1
#define false   0

#define DEBUG 0

#define WIDTH   1280
#define HEIGHT  640

#define ROWS    32
#define COLS    64

#define REGISTERS_SIZE 16
#define NUMBERS_OF_KEYS 16

#define FONT_SIZE 80

/*
    Load the Rom and fill the memory array
    Param : 1,pathname
*/
void LoadRom(char *game); 

/*
    Draw the rom at x and y position
    with a height of nibble
*/
void DrawSprite(uint8_t x, uint8_t y, uint8_t nibble);

/*
    Init memory and othe stuff
*/
void Init(void);

/*
    Simulate delay/sound
*/
void Tick(void);

/*
    Fetch ,decode and execute opcode
*/
void opcode_cycle(void);

#endif
