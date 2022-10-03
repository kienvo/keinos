#ifndef __PANIC_H__
#define __PANIC_H__

#define PANIC(fmt, ...) panic("%s:%d: " fmt,__FILE__, __LINE__, ##__VA_ARGS__)
void panic(const char *fmt, ...);

#endif /* __PANIC_H__ */
