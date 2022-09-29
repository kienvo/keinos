#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "io.h"
#include "console.h"
#include "tty.h"
#include "descriptor_table.h"

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler"
#endif

void kernel_main()
{
    con_init();
    GDT_init();
    IDT_init();
    // asm volatile("int $3");
    // asm volatile("int $4");

    outb(0x11, 0x20);
    outb(0x11, 0xA0);
    // ICW2
    outb(0x20, 0x21); // master: Vector offset, 0-7 -> 0x20-0x27 (32-39)
    outb(0x28, 0xA1); // slave: Vector offset, Vector, 8-15 -> 0x28-0x2F (40-47)
    // ICW2
    outb(0x04, 0x21); // master: Slave PIC at IRQ2 (0000 0100)
    outb(0x02, 0xA1); // slave: Slave id=2
    
    outb(0x05, 0x21); 
    outb(0x01, 0xA1);
	// Mask
    outb(0b11111101, 0x21);
    outb(0xff, 0xA1); 
    // EOI
	outb(0x20, 0x20);
    outb(0x20, 0xA0);

	rs_init();

    IDT_set_gate(32, (uint32_t)irq0, 0x08, 0x8e); // timer
    IDT_set_gate(33, (uint32_t)irq1, 0x08, 0x8e); // keyboard
    IDT_set_gate(34, (uint32_t)irq2, 0x08, 0x8e); // PIC2
    // IDT_set_gate(35, (uint32_t)irq3, 0x08, 0x8e); // COM2
    // IDT_set_gate(36, (uint32_t)irq4, 0x08, 0x8e); // COM1
    IDT_set_gate(37, (uint32_t)irq5, 0x08, 0x8e); // LPT2
    IDT_set_gate(38, (uint32_t)irq6, 0x08, 0x8e); // Floppy
    IDT_set_gate(39, (uint32_t)irq7, 0x08, 0x8e); // LPT1
    IDT_set_gate(40, (uint32_t)irq8, 0x08, 0x8e); // Real Time Clock
    IDT_set_gate(41, (uint32_t)irq9, 0x08, 0x8e); // General I/O
    IDT_set_gate(42, (uint32_t)irq10, 0x08, 0x8e);// General I/O
    IDT_set_gate(43, (uint32_t)irq11, 0x08, 0x8e);// General I/O
    IDT_set_gate(44, (uint32_t)irq12, 0x08, 0x8e);// General I/O
    IDT_set_gate(45, (uint32_t)irq13, 0x08, 0x8e);// Co-processor
    IDT_set_gate(46, (uint32_t)irq14, 0x08, 0x8e);// IDE bus
    IDT_set_gate(47, (uint32_t)irq15, 0x08, 0x8e);// IDE bus


	sti();
	rs_puts("ABCsdfsdfdfsd", COM1);
    while(1)
    {
        __asm__ volatile("hlt");
    }
}