#ifndef __PRINTK_H__
#define __PRINTK_H__

#include <stdarg.h>

#define PRINK_BUFSIZE (1024)

int printk(const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list args);
int vprintk(const char *fmt, va_list args);
int printk(const char *fmt, ...);


#endif /* __PRINTK_H__ */
