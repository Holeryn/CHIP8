e:
	gcc main.c chip8.c include/gfx.c -o chip8 -lX11 -lm
Work:
	gcc -Wall -g main.c chip8.c include/gfx.c -o chip8 -lX11 -lm