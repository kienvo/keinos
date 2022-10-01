#include "panic.h"
#include "printk.h"
#include <stdarg.h>

void panic(const char *fmt, ...)
{
	va_list arg;
	printk("\n*************KERNEL PANIC************\n");

	va_start(arg,fmt);
	vprintk(fmt, arg);
	va_end(arg);

	printk("\n");

	__asm__ volatile ("jmp hang");
}