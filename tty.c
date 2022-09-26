#include "tty.h"

void term_setcolor(uint8_t _color)
{
    color = _color;
}
void term_getpos(uint16_t x,uint16_t y)
{
	return;
}
int term_movtoy(uint8_t y)
{
    if(y > 25) return -1;
    posy = y;   
	return 0;
}
int term_movtox(uint8_t x)
{
    if(x > 80) return -1;
    posx = x;
	return 0;   
}

uint8_t term_gety()
{
    return posy;    
}
uint8_t term_getx()
{
    return posx;   
}
void term_movcur(uint16_t x, uint16_t y)
{
    if(x >= 80 || y >= 25) return;
    posx = x;
    posy = y;

    uint16_t pos = y*80+x;
    outb(0x3d4, 14);
    outb(0x3d5, (pos>>8)&0x00ff);
    outb(0x3d4, 15);
    outb(0x3d5, (pos)&0x00ff);
}
void term_putentryat(uint8_t _color,uint16_t x, uint16_t y, char c)
{
    uint16_t *term_buffer = (uint16_t*) 0xB8000 ;
    term_buffer[y*VGA_Width + x] = vga_entry(c,_color);
}

void term_scrollup(uint8_t NoRow2Mov)
{
    uint16_t *term_buffer = (uint16_t*) 0xB8000 ;
    for(uint16_t i = 0; i< VGA_Height*VGA_Width; i++)
        term_buffer[i] = term_buffer[i+VGA_Width * NoRow2Mov];
}

void term_putchar(char c)
{
    if(c=='\n') 
    {
        uint16_t temp = posy + 1;
        if(temp >= VGA_Height) 
        {
            term_scrollup(1);
            posy--;
            temp--;
        }
        while(posy < temp)
            term_putchar(' ');
        return;
    }

    else term_putentryat(color, posx, posy, c);

    if(++posx >= VGA_Width)
    {
        posx = 0;
        if(++posy >= VGA_Height)
        {
            term_scrollup(1);
            posy--;
        }
    }
}

void term_puts(const char * s)
{
    while(*s)
        term_putchar(*s++);
}

void term_clear()
{
    for(uint16_t i =0; i< VGA_Height*VGA_Width; i++)
        term_putchar(' ');
}

void term_init()
{
    posx = 0;
    posy = 0;
    color = vga_entry_color(DEFAULT_COLOR);

    term_clear();

    posx = 0;
    posy = 0;
}
