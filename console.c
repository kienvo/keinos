#include "console.h"

void con_setcolor(unsigned char _color)
{
    color = _color;
}
void con_getpos(unsigned short x,unsigned short y)
{
	return;
}
int con_movtoy(unsigned char y)
{
    if(y > 25) return -1;
    posy = y;   
	return 0;
}
int con_movtox(unsigned char x)
{
    if(x > 80) return -1;
    posx = x;
	return 0;   
}

unsigned char con_gety()
{
    return posy;    
}
unsigned char con_getx()
{
    return posx;   
}
void con_movcur(unsigned short x, unsigned short y)
{
    if(x >= 80 || y >= 25) return;
    posx = x;
    posy = y;

    unsigned short pos = y*80+x;
    outb(14, 0x3d4);
    outb((pos>>8)&0x00ff, 0x3d5);
    outb(15, 0x3d4);
    outb((pos)&0x00ff, 0x3d5);
}
void con_putentryat(unsigned char _color,unsigned short x, unsigned short y, char c)
{
    unsigned short *con_buffer = (unsigned short*) 0xB8000 ;
    con_buffer[y*VGA_Width + x] = vga_entry(c,_color);
}

static void con_scrollup(unsigned char NoRow2Mov)
{
    unsigned short *con_buffer = (unsigned short*) 0xB8000 ;
    for(unsigned short i = 0; i< VGA_Height*VGA_Width; i++)
        con_buffer[i] = con_buffer[i+VGA_Width * NoRow2Mov];
}

void con_putchar(char c)
{
    if(c=='\n') 
    {
        unsigned short temp = posy + 1;
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
    for(unsigned short i =0; i< VGA_Height*VGA_Width; i++)
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
