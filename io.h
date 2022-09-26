#ifndef _INCLUDED_IO_H_
#define _INCLUDED_IO_H_

#include <stdint.h>
extern void outb(uint16_t port, uint8_t val);
extern uint8_t inb(uint16_t port);

#endif //_INCLUDED_IO_H_