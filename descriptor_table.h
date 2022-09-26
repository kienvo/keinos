#ifndef _INCLUDED_DESCRIPTOR_TABLE_H_
#define _INCLUDED_DESCRIPTOR_TABLE_H_

#include <stdint.h>
#include "tty.h"

typedef struct
{
	uint16_t limit_l;
	uint16_t base_l;
	uint8_t base_m;
	uint8_t access;
	uint8_t gran;
	uint8_t base_h;
}__attribute__((packed)) GDT_t ;
typedef struct
{
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) GDT_ptr_t;


typedef struct 
{
	uint16_t base_l; // The lower 16 bits of the address to jump to when this interrupt fires
	uint16_t sel; 	// segment selector
	uint8_t always0;
	uint8_t flags;
	uint16_t base_h; // The upper 16 bits of the address to jump to
} __attribute__((packed)) IDT_t;
typedef struct  
{
	uint16_t size; // limit the idt entry
	uint32_t ptr2entry;
} __attribute__((packed)) IDT_ptr_t;


GDT_t GDT_entry[5];
GDT_ptr_t GDT_ptr;
IDT_t IDT_entry[256];
IDT_ptr_t IDT_ptr;

void IDT_init();
void GDT_init();
void isr_handler();
void IDT_set_gate(
	uint8_t num,
	uint32_t base,
	uint16_t sel, 
	uint8_t flags
);

extern int isr0();
extern int isr1();
extern int isr2();
extern int isr3();
extern int isr4();
extern int isr5();
extern int isr6();
extern int isr7();
extern int isr8();
extern int isr9();
extern int isr10();
extern int isr11();
extern int isr12();
extern int isr13();
extern int isr14();
extern int isr15();
extern int isr16();
extern int isr17();
extern int isr18();
extern int isr19();
extern int isr20();
extern int isr21();
extern int isr22();
extern int isr23();
extern int isr24();
extern int isr25();
extern int isr26();
extern int isr27();
extern int isr28();
extern int isr29();
extern int isr30();
extern int isr31();

extern void load_gdt(uint32_t *);
extern void load_idt(uint32_t *);

#endif