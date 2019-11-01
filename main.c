#include "chip8.h"
#include "include/gfx.h"
#include <X11/Xlib.h>

#define SQUARE_SIZE 10

#define EXPANDED_ROWS ROWS * SQUARE_SIZE
#define EXPANDED_COLS COLS * SQUARE_SIZE

void Draw(void);
void key_wait(unsigned char c,int type);

extern bool DrawFlag;
extern uint8_t video_memory[ROWS][COLS];
extern uint8_t font[FONT_SIZE];
extern uint8_t V[REGISTERS_SIZE];
extern uint8_t KEY_STATE[NUMBERS_OF_KEYS];

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

            gfx_clear();

            if(DrawFlag)
                Draw();
            
            /*Big for for simulate a delay*/
            for(i = 0; i <= 100000; i++)
                ;
        }else{
            x++;
        }
    }
}

void
Draw(void)
{
    int x,y;

    for(x = 0; x < ROWS; x++)
        for(y = 0; y < COLS ; y++)
            video_memory[x][y] ? gfx_point(y,x) : 0;
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
