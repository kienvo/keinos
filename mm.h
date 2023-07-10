#ifndef __PAGING_H__
#define __PAGING_H__

#include <stdint.h>
#include <stddef.h>
#include "common.h"

#include <stdint.h>

#define ALLOC_MAGIC ((uint32_t)0x12345678)

typedef struct 
{
	uint32_t magic;
	uint32_t is_hole;
	uint32_t sz;
} header_t;

typedef struct 
{
	uint32_t magic;
	header_t *header;
} footer_t;

typedef struct {
	footer_t prev_footer;
	header_t curr_header;
} contiguous_seg_t;

typedef struct {
	void *heap_base;
	uint32_t nchunks;
} heap_t;

// page aligned
#define kmalloc_a(sz) _kmalloc(sz, 1, NULL); 
// return physical address
#define kmalloc_p(sz, phys) _kmalloc(sz, 0, phys);
// page aligned + physical address
#define kmalloc_ap(sz, phys) _kmalloc(sz, 1, phys);

#define kmalloc(sz) _kmalloc(sz, 0, NULL);

// TODO: recheck this in linker.ld, waste memory, currently just use this
extern void __bss_end;

typedef struct page
{
	uint32_t present: 1;
	uint32_t rw     : 1;
	uint32_t user   : 1; //https://wiki.osdev.org/Paging#:~:text=U/S%2C%20the,page%20table%20entry.
	uint32_t accessed: 1;
	uint32_t dirty  : 1;
	uint32_t unused : 7;
	uint32_t frame  : 20;
} page_t;

typedef struct page_table
{
	page_t pages[1024];
} page_table_t;

typedef struct page_directory
{
	page_table_t *tables[1024]; // array of pointers
	uint32_t tables_phys[1024];
	uint32_t physical_addr;
} page_directory_t;

void paging_init(uint32_t memsize);
void switch_page_directory(page_directory_t *dir);
page_t *get_page(uint32_t addr, int create_new, page_directory_t *dir);
void alloc_frame(page_t *page, int is_user_page, int is_writeable);

void kheap_init();
uint32_t get_phys(uint32_t addr, page_directory_t *dir);
void kfree(void *p);


uint32_t _kmalloc(uint32_t sz, uint32_t is_align, uint32_t *phys);


#endif /* __PAGING_H__ */
