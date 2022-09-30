#include "panic.h"
#include "printk.h"
#include <stdarg.h>

void panic(const char *fmt, ...)
{
	va_list arg;
	va_start(arg,fmt);

	printk("\n*************KERNEL PANIC************\n");
	printk(fmt, arg);
	printk("\n");

	__asm__ volatile ("jmp hang");

	va_end(arg);
}