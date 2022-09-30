#include "printk.h"
#include "tty.h"
#include <stdarg.h>

static int default_tty = TTYS0;

/**
 * @return		int number of digits
 */
static int itoa( int value, char *str, int base, int is_upper ) {
	int i=0;
	int is_negative = value < 0;
	char buf[1024];
	const char *upper_digits = "0123456789ABCDEF";
	const char *lower_digits = "0123456789abcdef";
	const char *digits = is_upper ? upper_digits : lower_digits;
	if(is_negative) {
		value = -value;
	}
	while (value)
	{
		buf[i] = digits[value % base];
		value /= base;
		i++;
	}
	
	// revert buf[] to str[]
	str[0] = '-';
	str += is_negative;
	for(int j=0; j<i; j++) {
		str[i-j-1] = buf[j];
	}
	str[i] = 0; // string terminate

	return i+is_negative;
}

static int strlen(char *s)
{
	int i=0;
	while (i<1024 && *s++) {
		i++;
	}
	if (i==1024) return -1;
	else return i;
}

int printk(const char *fmt, ...)
{
	char buf[1024];
	char c, *s;
	int  n;

	va_list arg;
	va_start(arg, fmt);

	while (*fmt) {
		if(*fmt == '%') {
			switch (*(fmt+1))
			{
			case 'c':
				c = va_arg(arg, int);
				tty_write(default_tty, &c, 1);
				break;
			case 's':
				s = va_arg(arg, char *);
				int len = strlen(s);
				if(len<0) {
					return -1;
				}
				tty_write(default_tty, s, len);
				break;
			case 'd':
				n = itoa(va_arg(arg, int), buf, 10, 0);
				tty_write(default_tty, buf, n);
				break;
			case 'x':
				n = itoa(va_arg(arg, unsigned int), buf, 16, 0);
				tty_write(default_tty, buf, n);
				break;
			case 'X':
				n = itoa(va_arg(arg, unsigned int), buf, 16, 1);
				tty_write(default_tty, buf, n);
				break;
			
			default:
				break;
			}
			fmt += 2;
		} else {
			tty_write(default_tty, (char*)fmt, 1);
			fmt++;		
		}
	}
	char dummy = 0;
	tty_write(default_tty, &dummy, 1);
	va_end(arg);
	return 0;
}