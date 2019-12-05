#include "chip8.h"
#include "include/gfx.h"
#include <X11/Xlib.h>

#define SQUARE_SIZE 20

#define EXPANDED_ROWS ROWS * SQUARE_SIZE
#define EXPANDED_COLS COLS * SQUARE_SIZE

void Draw(void);
void paint(int row, int col, unsigned char color);
void key_wait(unsigned char c,int type);

extern bool DrawFlag;
extern uint8_t video_memory[ROWS][COLS];
extern uint8_t font[FONT_SIZE];
extern uint8_t V[REGISTERS_SIZE];
extern uint8_t KEY_STATE[NUMBERS_OF_KEYS];

uint8_t Extended_Vram[EXPANDED_ROWS][EXPANDED_COLS];

int
main(int argc, char *argv[])
{
    int x;
    int i;
    unsigned char  key_pressed,key_released;
    Init();
    LoadRom(SELECTED_ROM);
    gfx_open(WIDTH,HEIGHT,"Chip 8 Emulator");
    gfx_color(255,250,250);

    for(;;){
        if(!gfx_event_waiting(&key_pressed)){
            opcode_cycle();
            key_wait(key_released,0);
            
            #if DEBUG
                printf("# %d | %c #",x,key_pressed);
            #endif
            
            key_wait(key_pressed,1);
            key_released = key_pressed;

            if(DrawFlag){
                gfx_clear();
                Draw();
                DrawFlag = false;
            }

            for(i = 0; i <= 500000; i++)
                ;

        }else{
            x++;
        }
    }
}

void
Draw(void)
{
    register int row,col;
    
    for(row = 0; row < ROWS; row++){
        for(col = 0; col < COLS; col++){
            paint(row,col,video_memory[row][col] ? 1 : 0);
        }
    }
}


void paint(int row, int col, unsigned char color) {
    int pixel_row = row * SQUARE_SIZE;
    int pixel_col = col * SQUARE_SIZE;
    int drow, dcol;

    for (drow = 0; drow < SQUARE_SIZE; drow++) {
        for (dcol = 0; dcol < SQUARE_SIZE; dcol++) {
            color ? gfx_point(pixel_col + dcol, pixel_row + drow) : 0;
        }
    }
}


void
key_wait(unsigned char c,int type)
{
    switch (c)
    {
            case '1': KEY_STATE[1] = type ? 1 : 0; break;
            case '2': KEY_STATE[2] = type ? 1 : 0; break;
            case '3': KEY_STATE[3] = type ? 1 : 0; break;
            case '4': KEY_STATE[7] = type ? 1 : 0; break;
            case 'q': KEY_STATE[4] = type ? 1 : 0; break;
            case 'w': KEY_STATE[5] = type ? 1 : 0; break;
            case 'e': KEY_STATE[6] = type ? 1 : 0; break;
            case 'r': KEY_STATE[8] = type ? 1 : 0; break;
            case 'a': KEY_STATE[9] = type ? 1 : 0; break;
            case 's': KEY_STATE[0xA] = type ? 1 : 0; break;
            case 'd': KEY_STATE[0xB] = type ? 1 : 0; break;
            case 'f': KEY_STATE[0xC] = type ? 1 : 0; break;
            case 'g': KEY_STATE[0xD] = type ? 1 : 0; break;
            case 'z': KEY_STATE[0xE] = type ? 1 : 0; break;
            case 'x': KEY_STATE[0xF] = type ? 1 : 0; break;
    }
}
