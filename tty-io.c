#include "tty.h"
#include "console.h"


int tty_write(int channel, char *buf, int n)
{
	char *buf_base = buf;
	if((channel > 2) || (channel < 0) || (n < 0)) return -1;

	if(channel == TTYS0) {
		while(n>0) {
			rs_putchar(*buf++, COM1);
			n--;
		}
	}
	else if(channel == TTYS1) {
		while(n>0) {
			rs_putchar(*buf++, COM2);
			n--;
		}
	}
	else if(channel == TTY1)  {
		while(n>0) {
			con_putchar(*buf++);
			n--;
		}
	}
	return buf-buf_base;
}