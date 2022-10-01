#ifndef __TTY_H__
#define __TTY_H__

#define COM1 0x3F8
#define COM2 0x2F8

enum TTY_CHANNELS {
	TTYS0 = 0, // serial 0
	TTYS1 = 1, // serial 1
	TTY1 = 2   // console
};

void rs_init();

void rs_putchar(char c, int port);
void rs_puts(char* s, int port);

int tty_write(int channel, char *buf, int n);

#endif /* __TTY_H__ */
