#ifndef _INCLUDED_TTY_H_ 
#define _INCLUDED_TTY_H_ 

#include <stdint.h>
#include "io.h"

enum vga_color
{	
    VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4, 
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,    
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}
static inline uint16_t vga_entry(char c, uint8_t color)
{
    return (uint16_t)c | (uint16_t)color << 8;
}


#define DEFAULT_COLOR VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK
//#define DEFAULT_COLOR VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK
static const uint16_t VGA_Width = 80, VGA_Height = 25;
static uint16_t posy, posx, color;


void con_setcolor(uint8_t _color);
void con_getpos(uint16_t x,uint16_t y);
int con_movtoy(uint8_t y);
int con_movtox(uint8_t x);
uint8_t con_gety();
uint8_t con_getx();

void con_movcur(uint16_t x, uint16_t y);
void con_putentryat(uint8_t color,uint16_t x, uint16_t y, char c);
void con_putchar(char c);
void con_puts(const char * s);
void con_clear();
void con_init();

#endif //_INCLUDED_TTY_H_