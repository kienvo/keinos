#include "tty.h"
#include "io.h"
#include "common.h"

static void init(int port)
{
	outb(0x00, port+1); // disable interrupt
	outb(0x80, port+3); // enable DLAB
	outb(0x01, port  ); // baud 115200
	outb(0x00, port+1); // baud 115200
	outb(0x03, port+3); // reset DLAB, 8n1
	outb(0x0b, port+4); // set DTR, RTS
	outb(0x0d, port+4); // enable interrupt except writes
	inb(port); // test	
}
void rs1_irq(reg_t regs)
{
	
}
void rs2_irq(reg_t regs)
{
	
}
void rs_init()
{
	IDT_set_gate(36, (uint32_t)irq4, 0x08, 0x8e);
	IDT_set_gate(35, (uint32_t)irq3, 0x08, 0x8e);
	init(COM1);
	init(COM2);
	outb(inb(0x21)&0xE7, 0x21); // set mask, 0xE7 (11100111)
}

static int is_transmit_empty(int port)
{
	return inb(port+5) & 0x20; // bit5
}

void rs_putchar(char c, int port)
{
	cli();
	while (!is_transmit_empty(port));
	outb(c, port);
	sti();
}
void rs_puts(char* s, int port)
{
    while(*s)
        rs_putchar(*s++, port);	
}