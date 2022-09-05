#include "chip8.h"

#define STACK_SIZE 48
#define MEMORY_SIZE 4096

#define SIZE ROWS*COLS

#define MAX_GAME_SIZE (0x1000 - START_PROGRAM_ADDRESS)

#define START_PROGRAM_ADDRESS 0x200
#define FONT_OFFSET		0x00

#define START_CALL_STACK 0xEA0
#define END_CALL_STACK 0xEFF

#define START_DISPLAY_REFRESH_OFFSET 0xF00
#define END_DISPLAY_REFRESH_OFFSET 0xFFF

#define REGISTER_SIZE 16
#define KEY_SIZE 16

uint16_t    PC;
uint16_t    opcode;
uint16_t    I;
uint16_t    SP;
uint16_t    stack[STACK_SIZE];


uint8_t     video_memory[ROWS][COLS];
uint8_t     memory[MEMORY_SIZE];
uint8_t     V[REGISTERS_SIZE];
uint8_t     delay_timer;
uint8_t     sound_timer;
uint8_t     KEY_STATE[NUMBERS_OF_KEYS];

bool        DrawFlag;

/* Standard 4x5 font */
static uint16_t font[FONT_SIZE] = {
/* '0' */ 0xF0, 0x90, 0x90, 0x90, 0xF0,
/* '1' */ 0x20, 0x60, 0x20, 0x20, 0x70,
/* '2' */ 0xF0, 0x10, 0xF0, 0x80, 0xF0,
/* '3' */ 0xF0, 0x10, 0xF0, 0x10, 0xF0,
/* '4' */ 0x90, 0x90, 0xF0, 0x10, 0x10,
/* '5' */ 0xF0, 0x80, 0xF0, 0x10, 0xF0,
/* '6' */ 0xF0, 0x80, 0xF0, 0x90, 0xF0,
/* '7' */ 0xF0, 0x10, 0x20, 0x40, 0x40,
/* '8' */ 0xF0, 0x90, 0xF0, 0x90, 0xF0,
/* '9' */ 0xF0, 0x90, 0xF0, 0x10, 0xF0,
/* 'A' */ 0xF0, 0x90, 0xF0, 0x90, 0x90,
/* 'B' */ 0xE0, 0x90, 0xE0, 0x90, 0xE0,
/* 'C' */ 0xF0, 0x80, 0x80, 0x80, 0xF0,
/* 'D' */ 0xE0, 0x80, 0x80, 0x80, 0xE0,
/* 'E' */ 0xF0, 0x80, 0xF0, 0x80, 0xF0,
/* 'F' */ 0xF0, 0x80, 0xF0, 0x80, 0x80,
};


void
opcode_cycle(void)
{
    uint8_t     a;
    uint8_t     X;
    uint8_t     Y;
    uint8_t     Z;

    uint16_t     NN;
    uint16_t    NNN; 

    int     i;

/*
    In caso non andasse ricordati di vedere se fread ritorna
    quantitativo di byte letti e scritti nella memoria apposto di usare MAX_GAME_SIZE
*/  
    opcode = memory[PC] << 8 | memory[PC + 1];
    /*Take the first digit of the opcode*/
    a   = (opcode & 0xF000) >> 0xC;
    NNN = (opcode & 0x0FFF);
    NN  = (opcode & 0x00FF);

    X = (opcode & 0x0F00) >> 0x8;   /*Second Digit of the opcode*/
    Y = (opcode & 0x00F0) >> 0x4;   /*Third Digit of the opcode*/
    Z = (opcode & 0x000F);          /*Last Digit of the opcode*/

    #if DEBUG
        printf("%x \n",opcode);
    #endif

// Ok, i REALLY don't know why i used if/elif/else statemente instead of switch case
// there is a major difference in the two (che the asm)
// but it's a boring correction and i don't want to do it :-).
      if(a == 0){
    /*
        Clear Istruction
    */
        if(NNN == 0x0E0){
            memset(video_memory, 0, sizeof(uint8_t) * (SIZE));
            DrawFlag = true;
            PC += 2;
        }
    /*
        Return Istruction
    */
        else if(NNN == 0x0EE)
            PC = stack[--SP];
    }
/*
    Jump to address NNN
*/
    else if(a == 1)
        PC = NNN;
/*
    Call Adrress NNN
*/
    else if(a == 2){
        stack[SP++] = PC + 2;
        PC = NNN;
    }
/*
    Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block) 
*/
    else if(a == 3){
        if(V[X] == NN)
            PC += 4;
        else
            PC += 2;
    }
/*
    Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
*/
    else if(a == 4){
        if(V[X] != NN)
            PC += 4;
        else 
            PC += 2;
    }
/*
    Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block) 
*/
    else if(a == 5){
        if(V[X] == V[Y])
            PC += 4;
        else 
            PC += 2;
    }
/*
    Sets VX to NN. 
*/
    else if(a == 6){
        V[X] = NN;
        PC += 2;
    }
/*
    ADDS NN to VX
*/
    else if(a == 7){
        V[X] += NN;
        PC += 2;
    }
/*
    Sets VX to the value of VY. 
*/
    if(a == 8){
        if(Z == 0){
            V[X] = V[Y];
            PC += 2;
        }
/*
    Sets VX to VX or VY. (Bitwise OR operation) 
*/
        else if(Z == 1){
            V[X] |= V[Y];
            PC += 2;
        }
/*
    Sets VX to VX and VY. (Bitwise AND operation) 
*/
        else if(Z == 2){
            V[X] &= V[Y];
            PC += 2;
        }
/*
    Sets VX to VX xor VY. 
*/
        else if(Z == 3){
            V[X] ^= V[Y];
            PC += 2;
        }
/*
    Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't. 
*/
        else if(Z == 4){
            V[0xF] = ((int) V[X] + (int) V[Y]) > 255 ? 1 : 0;
            V[X] += V[Y];
            PC += 2;
        }
/*
    VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't. 
*/
        else if(Z == 5){
            V[0xF] = (V[X] > V[Y]) ? 1 : 0;
            V[X] -= V[Y];
            PC += 2;
        }
/*
    Stores the least significant bit of VX in VF and then shifts VX to the right by 1
*/
        else if(Z == 6){
            V[0xF] = V[X] & 0x1;
            V[X] >>= 1;
            PC += 2;
        }
/*
    Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't. 
*/
        else if(Z == 7){
            V[0xF] = (V[Y] > V[X]) ? 1 : 0;
            V[X] = V[Y] - V[X];
            PC += 2;
        }
/*
    Stores the most significant bit of VX in VF and then shifts VX to the left by 1
*/
        else if(Z == 0xE){
            V[0xF] = (V[X] >> 7) & 0x1;
            V[X] <<= 1;
            PC += 2;
        }
    }
/*
    Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block) 
*/
    else if(a == 9){
        if(V[X] != V[Y])
            PC += 4;
        else
            PC += 2;
    }
    
/*
    Sets I to the address NNN. 
*/
    else if(a == 0xA){
        I = NNN;
        PC += 2;
    }
/*
    Jumps to the address NNN plus V0. 
*/
    else if(a == 0xB)
        PC = V[0] + NNN;
/*
    Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN. 
*/
    else if(a == 0xC){
        V[X] = rand() & NN;
        PC += 2;
    }
/*
    Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; 
    I value doesn’t change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from 
    set to unset when the sprite is drawn, and to 0 if that doesn’t happen 
*/
    else if(a == 0xD){
        DrawSprite(V[X],V[Y],Z); /*X,Y,HEIGHT*/
        PC += 2;
        DrawFlag = true;
    }
    else if(a == 0xE){
/*
    Skips the next instruction if the key stored in VX is pressed. (Usually the next 
    instruction is a jump to skip a code block) 
*/
        if(NN == 0x9E){
             if(KEY_STATE[V[X]])
                PC += 4;
            else
                PC += 2;
        }
/*
    Skips the next instruction if the key stored in VX isn't pressed. (Usually the next 
    instruction is a jump to skip a code block) 
*/
        else if(NN == 0xA1){
             if(!KEY_STATE[V[X]])
                PC += 4;
            else
                PC += 2;
        }
    }

     else if(a == 0xF){
/*
    Sets VX to the value of the delay timer. 
*/
        if(NN == 0x07){
            V[X] = delay_timer;
            PC += 2;
        }
/*
    A key press is awaited, and then stored in VX. (Blocking
    Operation.All instruction halted until next key event) 
*/
        else if(NN == 0x0A){
            V[X] = gfx_wait();
            PC += 2;
        }
/*
    Sets the delay timer to VX. 
*/
        else if(NN == 0x15){
            delay_timer = V[X];
            PC += 2;
        }
/*
    Sets the sound timer to VX
*/
        else if(NN == 0x18){
            sound_timer = V[X];
            PC += 2;
        }
/*
    Adds VX to I
*/
        else if(NN == 0x1E){
            I += V[X];
            PC += 2;
        }
/*
    Sets I to the location of the sprite for the character in VX. 
    Characters 0-F (in hexadecimal) are represented by a 4x5 font. 
*/
        else if(NN == 0x29){
            I = 5 * V[X];
            PC += 2;
        }
/*
    Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2.
    (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.) 
*/
        else if(NN == 0x33){
            memory[I]   = (V[X] % 1000) / 100;
            memory[I+1] = (V[X] % 100) / 10;
            memory[I+2] = (V[X] % 10);
            PC += 2;
/*
    Stores V0 to VX (including VX) in memory starting at address I. The offset from I is
    increased by 1 for each value written, but I itself is left unmodified. 
*/
        }else if(NN == 0x55){
            for(i = 0; i <= X; i++)
                memory[I+i] = V[i];
            I = I + ( X + 1);
            PC += 2;
/*
    Fills V0 to VX (including VX) with values from memory starting at address I.
    The offset from I is increased by 1 for each value written, but I itself is left unmodified. 
*/
        }else if(NN == 0x65){
            for(i = 0; i <= X; i++)
                V[i] = memory[I + i];
            I = I + (X + 1);
            PC += 2;
        }
        Tick();
    }
}

void 
LoadRom(char *game){
    FILE *fp;

    fp = fopen(game, "rb");
    
    if (fp == NULL) {
        fprintf(stderr, "failed to open The game: %s\n", game);
        exit(EXIT_FAILURE);
    }

    fread(&memory[START_PROGRAM_ADDRESS], 1, MAX_GAME_SIZE, fp);

    fclose(fp);
}

void 
DrawSprite(uint8_t x, uint8_t y, uint8_t nibble)
{
    unsigned row = y, col = x;
    unsigned byte_index;
    unsigned bit_index;

    V[0xF] = 0;
    for (byte_index = 0; byte_index < nibble; byte_index++) {
        uint8_t byte = memory[I + byte_index];

        for (bit_index = 0; bit_index < 8; bit_index++) {
            uint8_t bit = (byte >> bit_index) & 0x1;
            uint8_t *pixelp = &video_memory[(row + byte_index) % ROWS]
                                  [(col + (7 - bit_index)) % COLS];

            if (bit == 1 && *pixelp ==1) V[0xF] = 1;

            *pixelp = *pixelp ^ bit;
        }
    }
}

void Init(void)
{
    int i;

    PC     = 0x200;
    opcode = 0;
    I      = 0;
    SP     = 0;

    memset(stack,           0, sizeof(uint16_t) * STACK_SIZE);
    memset(V,               0, sizeof(uint8_t)  * REGISTER_SIZE);
    memset(memory,          0, sizeof(uint8_t)  * MEMORY_SIZE);
    memset(video_memory,    0, sizeof(uint8_t)  * SIZE);
    memset(font,            0, sizeof(uint8_t)  * KEY_SIZE);

    for (i = 0; i < 80; i++)
        memory[FONT_OFFSET + i] = font[i];

    DrawFlag = true;
    delay_timer = 0;
    sound_timer = 0;
    srand(time(NULL));
}

void Tick(void) {
    if(delay_timer > 0)
        --delay_timer;

    if(sound_timer > 0){
        --sound_timer;
        #if DEBUG
            if(sound_timer == 0)
                printf("\aBeep");
        #endif
    }
}
