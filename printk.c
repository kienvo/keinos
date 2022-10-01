#include "printk.h"
#include "tty.h"

static int default_tty = TTYS0;

/**
 * @return		int number of digits
 */
static int itoa( int value, char *str, int base, int is_upper ) {
	int i=0;
	int is_negative = value < 0;
	char buf[PRINK_BUFSIZE];
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
	while (i<PRINK_BUFSIZE && *s++) {
		i++;
	}
	if (i==PRINK_BUFSIZE) return -1;
	else return i;
}

static int memcpy ( void * dest, const void * src, int num )
{
	// TODO: needs optimize
	char *d = dest;
	const char *s = src;
	int b=0;
	for(int i=0; i<num; i++) {
		*d = *s;
		d++;
		s++;
		b++;
	}
	return b;
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
	char *buf_base = buf;
	char str[PRINK_BUFSIZE];
	char c, *s;
	int  n;
	while (*fmt) {
		if(*fmt == '%') {
			switch (*(fmt+1))
			{
			case 'c':
				c = va_arg(args, int);
				*buf = c;
				buf++;
				break;
			case 's':
				s = va_arg(args, char *);
				int len = strlen(s);
				if(len<0) {
					return -1;
				}
				memcpy(buf, s, len);
				buf += len;
				break;
			case 'd':
				n = itoa(va_arg(args, int), str, 10, 0);
				memcpy(buf, str, n);
				buf += n;
				break;
			case 'x':
				n = itoa(va_arg(args, unsigned int), str, 16, 0);
				memcpy(buf, str, n);
				buf += n;
				break;
			case 'X':
				n = itoa(va_arg(args, unsigned int), str, 16, 1);
				memcpy(buf, str, n);
				buf += n;
				break;
			
			default:
				*buf = 0;
				return buf - buf_base; //Something wrong! abort.
				break;
			}
			fmt += 2;
		} else {
			*buf = *fmt;
			buf++;
			fmt++;
		}
	}
	*buf = 0;
	return buf - buf_base; 
}

int vprintk(const char *fmt, va_list args)
{
	char buf[PRINK_BUFSIZE];

	int len = vsprintf(buf, fmt, args);
	tty_write(default_tty, buf, len);

	return len;
}

int printk(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	int len = vprintk(fmt, args);
	va_end(args);

	return len;
}