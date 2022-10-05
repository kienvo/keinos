#include "mm.h"
static uint32_t placement_addr = (uint32_t)&__bss_end;

uint32_t _kmalloc(uint32_t sz, uint32_t is_align, uint32_t *phys)
{
	if(is_align && (placement_addr & 0xFFFFF000)) {
		placement_addr &= 0xFFFFF000;
		placement_addr += 0x1000;
	}
	if (phys) {
		*phys = placement_addr;
	}
	uint32_t ret = placement_addr;
	placement_addr += sz;
	return ret;
}