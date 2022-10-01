#ifndef __DEBUG_H__
#define __DEBUG_H__


#if DEBUG_LEVEL == 0
#define RETURN_ERR(x) return x

#elif DEBUG_LEVEL == 1
#include "printk.h"
#define RETURN_ERR(x) {printk("warning: Fail in %s:%d\n", __FILE__, __LINE__); return x;}

#elif DEBUG_LEVEL == 2
#include "panic.h"
#define RETURN_ERR(x) panic("warning: Fail in %s:%d\n", __FILE__, __LINE__)

#else
#define RETURN_ERR(x) return x
#endif

#endif /* __DEBUG_H__ */
