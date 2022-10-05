#include "mm.h"
#include "panic.h"
#include "string.h"

extern uint32_t placement_addr = (uint32_t)&__bss_end;
uint32_t *frames;
uint32_t nframes;

#define INDEX_FROM_BIT(a) (a/(32))
#define OFFSET_FROM_BIT(a) (a%(32))

static void set_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr/ 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t offs = OFFSET_FROM_BIT(frame);
	frames[idx] |= (1 << offs);
}

static void clear_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr/ 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t offs = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(1 << offs);
}

static uint32_t test_frame(uint32_t frame_addr)
{
	uint32_t frame = frame_addr/ 0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t offs = OFFSET_FROM_BIT(frame);
	return frames[idx] & (1 << offs);
}

static uint32_t get_free_frame()
{
	uint32_t i=0;
	uint32_t j=0;
	for(; i<INDEX_FROM_BIT(nframes); i++) {
		if(frames[i] != 0xFFFFFFFF) {
			for(j=0; j<32; j++) {
				if(!(frames[i] & (1<<j))) {
					return (i*32)+j;
				}
			}
		}
	}
	return -1;
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable) 
{
	if (page->frame != 0) {
		return; // frame was already allocated
	} else {
		uint32_t idx = get_free_frame();
		if(idx == (uint32_t)-1) {
			PANIC("No free frames!");
		}
		set_frame(idx*0x1000);
		page->present = 1;
		page->rw      = is_writeable != 0;
		page->user    = is_kernel    != 0;
		page->frame   = idx;
	}
}

void do_free_frame(page_t *page) {
	if(page->frame) {
		return;
	} else {
		clear_frame(page->frame);
		page->frame = 0x00;
	}
}

void paging_init(uint32_t memend) 
{
	placement_addr = (uint32_t)&__bss_end; // free memory start at the end of .bss section
	nframes = memend/0x1000;
	frames  = (uint32_t*) kmalloc(INDEX_FROM_BIT(nframes)*sizeof(uint32_t));
	memset(frames, 0, INDEX_FROM_BIT(nframes)*sizeof(uint32_t));

	page_directory_t *kernel_directory = 
			(page_directory_t*)kmalloc_a(sizeof(page_directory_t));
	memset(kernel_directory, 0, (sizeof(page_directory_t)));

	uint32_t i=0;
	while (i<placement_addr)
	{
		// prevent write access from user-space
		alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
		i += 0x1000;
	}
	
	// IDT_set_gate(14,(uint32_t)isr14,0x08,0x8e);
	switch_page_directory(kernel_directory);
}

void switch_page_directory(page_directory_t *dir)
{
	__asm__ volatile("mov %0, %%cr3":: "r"(&dir->tables_phys));
	uint32_t cr0;
	__asm__ volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000; // Enable paging!
	__asm__ volatile("mov %0, %%cr0":: "r"(cr0));
}

page_t *get_page(
	uint32_t addr, 
	int create_new, 
	page_directory_t *dir)
{
	addr /= 0x1000;
	uint32_t tbidx = addr / 1024;
	if (dir->tables[tbidx]) {
		return &dir->tables[tbidx]->pages[addr%1024];
	} else if (create_new) {
		uint32_t tmp;
		dir->tables[tbidx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
		memset(dir->tables[tbidx], 0, 0x1000);
		dir->tables_phys[tbidx] = tmp | 0x07; // present, rw, user
		return &dir->tables[tbidx]->pages[addr%1024];
	} else {
		return 0;
	}
}