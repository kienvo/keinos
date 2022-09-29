#include "console.h"

void con_setcolor(uint8_t _color)
{
    color = _color;
}
void con_getpos(uint16_t x,uint16_t y)
{
	return;
}
int con_movtoy(uint8_t y)
{
    if(y > 25) return -1;
    posy = y;   
	return 0;
}
int con_movtox(uint8_t x)
{
    if(x > 80) return -1;
    posx = x;
	return 0;   
}

uint8_t con_gety()
{
    return posy;    
}
uint8_t con_getx()
{
    return posx;   
}
void con_movcur(uint16_t x, uint16_t y)
{
    if(x >= 80 || y >= 25) return;
    posx = x;
    posy = y;

    uint16_t pos = y*80+x;
    outb(14, 0x3d4);
    outb((pos>>8)&0x00ff, 0x3d5);
    outb(15, 0x3d4);
    outb((pos)&0x00ff, 0x3d5);
}
void con_putentryat(uint8_t _color,uint16_t x, uint16_t y, char c)
{
    uint16_t *con_buffer = (uint16_t*) 0xB8000 ;
    con_buffer[y*VGA_Width + x] = vga_entry(c,_color);
}

static void con_scrollup(uint8_t NoRow2Mov)
{
    uint16_t *con_buffer = (uint16_t*) 0xB8000 ;
    for(uint16_t i = 0; i< VGA_Height*VGA_Width; i++)
        con_buffer[i] = con_buffer[i+VGA_Width * NoRow2Mov];
}

void con_putchar(char c)
{
    if(c=='\n') 
    {
        uint16_t temp = posy + 1;
        if(temp >= VGA_Height) 
        {
            con_scrollup(1);
            posy--;
            temp--;
        }
        while(posy < temp)
            con_putchar(' ');
        return;
    }

    else con_putentryat(color, posx, posy, c);

    if(++posx >= VGA_Width)
    {
        posx = 0;
        if(++posy >= VGA_Height)
        {
            con_scrollup(1);
            posy--;
        }
    }
}

void con_puts(const char * s)
{
    while(*s)
        con_putchar(*s++);
}

void con_clear()
{
    for(uint16_t i =0; i< VGA_Height*VGA_Width; i++)
        con_putchar(' ');
}

void con_init()
{
    posx = 0;
    posy = 0;
    color = vga_entry_color(DEFAULT_COLOR);

    con_clear();

    posx = 0;
    posy = 0;
}
