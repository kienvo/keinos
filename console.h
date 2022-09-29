#ifndef _INCLUDED_TTY_H_ 
#define _INCLUDED_TTY_H_ 

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

static inline unsigned char vga_entry_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}
static inline unsigned short vga_entry(char c, unsigned char color)
{
    return (unsigned short)c | (unsigned short)color << 8;
}


#define DEFAULT_COLOR VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK
//#define DEFAULT_COLOR VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK
static const unsigned short VGA_Width = 80, VGA_Height = 25;
static unsigned short posy, posx, color;


void con_setcolor(unsigned char _color);
void con_getpos(unsigned short x,unsigned short y);
int con_movtoy(unsigned char y);
int con_movtox(unsigned char x);
unsigned char con_gety();
unsigned char con_getx();

void con_movcur(unsigned short x, unsigned short y);
void con_putentryat(unsigned char color,unsigned short x, unsigned short y, char c);
void con_putchar(char c);
void con_puts(const char * s);
void con_clear();
void con_init();

#endif //_INCLUDED_TTY_H_