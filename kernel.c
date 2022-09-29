#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "io.h"
#include "console.h"
#include "tty.h"
#include "common.h"

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler"
#endif

void kernel_main()
{
    GDT_init();
    IDT_init();
	pic_init();
    
	con_init();
	rs_init();

	sti();
	rs_puts("ABCsdfsdfdfsd", COM1);
    while(1)
    {
        __asm__ volatile("hlt");
    }
}