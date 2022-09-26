#include "descriptor_table.h"



static void GDT_set_gate
(
	uint32_t num,
	uint32_t base,
	uint32_t limit,
	uint8_t access,
	uint8_t gran
)
{
	GDT_entry[num].base_l = base & 0xffff;
	GDT_entry[num].base_m = (base>>16) & 0xff;
	GDT_entry[num].base_h = (base>>24) & 0xff;

	GDT_entry[num].limit_l = limit & 0xffff;
	GDT_entry[num].gran = (limit >> 16) & 0x0f;

	GDT_entry[num].gran |= gran & 0xf0;
	GDT_entry[num].access = access;
}

void GDT_init()
{
	GDT_ptr.limit = sizeof(GDT_t)*5-1;
	GDT_ptr.base = (uint32_t)GDT_entry;

	GDT_set_gate(0,0,0,0,0);
	GDT_set_gate(1, 0, 0xffffffff,0x9a, 0xcf);
	GDT_set_gate(2, 0, 0xffffffff,0x92, 0xcf);
	GDT_set_gate(3, 0, 0xffffffff,0xfa, 0xcf);
	GDT_set_gate(4, 0, 0xffffffff,0xf2, 0xcf);

	load_gdt((uint32_t*)&GDT_ptr);
}

void IDT_set_gate(
	uint8_t num,
	uint32_t base,
	uint16_t sel, 
	uint8_t flags
)
{
	IDT_entry[num].base_l = base & 0x0000ffff;
	IDT_entry[num].base_h = (base>>16) & 0x0000ffff;

	IDT_entry[num].sel = sel;
	IDT_entry[num].always0 = 0;
	IDT_entry[num].flags = flags;
}

__attribute__((interrupt)) void isr_handler_iret(uint16_t *dummy);
void IDT_init()
{

	//IDT_ptr.size = sizeof(IDT_t) * 0x11;
	IDT_ptr.size = sizeof(IDT_t) * 256 - 1;
	IDT_ptr.ptr2entry = (uint32_t)IDT_entry;

	uint8_t *tmp = (void*)IDT_entry;
	for(uint32_t i = 0; i < sizeof(IDT_t)*256-1; i++)
		tmp[i] = 0;

	IDT_set_gate(0,(uint32_t)isr0,0x08,0x8e);
	IDT_set_gate(1,(uint32_t)isr1,0x08,0x8e);
	IDT_set_gate(2,(uint32_t)isr2,0x08,0x8e);
	IDT_set_gate(3,(uint32_t)isr3,0x08,0x8e);
	IDT_set_gate(4,(uint32_t)isr4,0x08,0x8e);
	IDT_set_gate(5,(uint32_t)isr5,0x08,0x8e);
	IDT_set_gate(6,(uint32_t)isr6,0x08,0x8e);
	IDT_set_gate(7,(uint32_t)isr7,0x08,0x8e);
	IDT_set_gate(8,(uint32_t)isr8,0x08,0x8e);
	IDT_set_gate(9,(uint32_t)isr9,0x08,0x8e);
	IDT_set_gate(10,(uint32_t)isr10,0x08,0x8e);
	IDT_set_gate(11,(uint32_t)isr11,0x08,0x8e);
	IDT_set_gate(12,(uint32_t)isr12,0x08,0x8e);
	IDT_set_gate(13,(uint32_t)isr13,0x08,0x8e);
	IDT_set_gate(14,(uint32_t)isr14,0x08,0x8e);
	IDT_set_gate(15,(uint32_t)isr15,0x08,0x8e);
	IDT_set_gate(16,(uint32_t)isr16,0x08,0x8e);
	IDT_set_gate(17,(uint32_t)isr17,0x08,0x8e);
	IDT_set_gate(18,(uint32_t)isr18,0x08,0x8e);
	IDT_set_gate(19,(uint32_t)isr19,0x08,0x8e);
	IDT_set_gate(20,(uint32_t)isr20,0x08,0x8e);
	IDT_set_gate(21,(uint32_t)isr21,0x08,0x8e);
	IDT_set_gate(22,(uint32_t)isr22,0x08,0x8e);
	IDT_set_gate(23,(uint32_t)isr23,0x08,0x8e);
	IDT_set_gate(24,(uint32_t)isr24,0x08,0x8e);
	IDT_set_gate(25,(uint32_t)isr25,0x08,0x8e);
	IDT_set_gate(26,(uint32_t)isr26,0x08,0x8e);
	IDT_set_gate(27,(uint32_t)isr27,0x08,0x8e);
	IDT_set_gate(28,(uint32_t)isr28,0x08,0x8e);
	IDT_set_gate(29,(uint32_t)isr29,0x08,0x8e);
	IDT_set_gate(30,(uint32_t)isr30,0x08,0x8e);
	IDT_set_gate(31,(uint32_t)isr31,0x08,0x8e);

	load_idt((uint32_t*)&IDT_ptr);
}
typedef struct registers
{
   uint32_t ds;                  // Data segment selector
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
   uint32_t int_no, err_code;    // Interrupt number and error code (if applicable)
   //uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} reg_t;

void isr_handler( int_no)
uint32_t int_no;
{
	term_puts("interrupt: ");
	term_putchar(int_no/100+48);
	term_putchar((int_no%100)/10+48);
	term_putchar(int_no%10+48);
	term_putchar('\n');
}

__attribute__((interrupt)) void isr_handler_iret(uint16_t *dummy)
{
	term_puts("isr_handler_iret: \n");
}
