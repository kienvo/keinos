#ifndef _INCLUDED_DESCRIPTOR_TABLE_H_
#define _INCLUDED_DESCRIPTOR_TABLE_H_

#include <stdint.h>

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
typedef struct registers
{
   uint32_t ds;                  // Data segment selector
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
   uint32_t int_no, err_code;    // Interrupt number and error code (if applicable)
   uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} reg_t;

GDT_t GDT_entry[5];
GDT_ptr_t GDT_ptr;
IDT_t IDT_entry[256];
IDT_ptr_t IDT_ptr;

void IDT_init();
void GDT_init();
void pic_init();
void isr_handler(reg_t);
void irq_handler(reg_t);
void isr_register_handler(int int_no, void *handler);
void IDT_set_gate(
	uint8_t num,
	uint32_t base,
	uint16_t sel, 
	uint8_t flags
);

int isr0();
int isr1();
int isr2();
int isr3();
int isr4();
int isr5();
int isr6();
int isr7();
int isr8();
int isr9();
int isr10();
int isr11();
int isr12();
int isr13();
int isr14();
int isr15();
int isr16();
int isr17();
int isr18();
int isr19();
int isr20();
int isr21();
int isr22();
int isr23();
int isr24();
int isr25();
int isr26();
int isr27();
int isr28();
int isr29();
int isr30();
int isr31();

int irq0();
int irq1();
int irq2();
int irq3();
int irq4();
int irq5();
int irq6();
int irq7();
int irq8();
int irq9();
int irq10();
int irq11();
int irq12();
int irq13();
int irq14();
int irq15();

void load_gdt(uint32_t *);
void load_idt(uint32_t *);

#endif