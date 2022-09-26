#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
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
    term_init();
    GDT_init();
    IDT_init();
    asm volatile("int $2");
    asm volatile("int $2");
    asm volatile("int $3");
    asm volatile("int $4");

    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    // ICW2
    outb(0x21, 0x20); // Matster Vector offset, 0-7 -> 0x20-0x27 (32-39)
    outb(0xA1, 0x28); // Matster Vector offset, Vector, 8-15 -> 0x28-0x2F (40-47)
    // ICW2
    outb(0x21, 0x04); // Slave PIC at IRQ2 (0000 0100)
    outb(0xA1, 0x02); // 
    
    outb(0x21, 0x01); 
    outb(0xA1, 0x01);
    
    // Active IRQs
    outb(0x21, 0xFC); // Unmask Mater IRQs
    //outb(0xA1, 0x00);
    
    IDT_set_gate(32, (uint32_t)isr5, 0x08, 0x8e);
    IDT_set_gate(33, (uint32_t)isr4, 0x08, 0x8e);
    IDT_set_gate(34, (uint32_t)isr3, 0x08, 0x8e);
    IDT_set_gate(35, (uint32_t)isr3, 0x08, 0x8e);
    IDT_set_gate(36, (uint32_t)isr3, 0x08, 0x8e);
    IDT_set_gate(37, (uint32_t)isr3, 0x08, 0x8e);
    IDT_set_gate(38, (uint32_t)isr3, 0x08, 0x8e);

    outb(0x20, 0x20);

    inb(0x60);
    //outb(0x64, 0xAE);
    //outb(0x20, 0x20);
    inb(0x60);
    inb(0x60);

    asm volatile ( "outb %%al, $0x80" : : "a"(0) );
    while(1)
    {
        asm volatile("hlt");
        term_putchar(inb(0x60));
        term_putchar(inb(0x60));
        outb(0x20, 0x20);
    }
}