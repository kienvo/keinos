/**
 * @file		kheap.c
 * @author		kienvo (kein@kienlab.com)
 * @brief 		kernel heap manager
 * @version		0.1
 * @date		Oct-11-2022
 * @copyright	Copyright (c) 2022 by kein@kienlab.com
 * 
 */
// TODO: check for memory leaks

#include "mm.h"
#include "panic.h"

extern page_directory_t *kernel_directory;
uint32_t placement_addr;
heap_t *kheap = NULL;

// Calculate footer pointer from header pointer
#define HSIZE (sizeof(header_t))
#define FSIZE (sizeof(footer_t))
#define CAL_FOOTER(h_p) (footer_t*)((uint32_t)h_p + HSIZE + ((header_t*)(h_p))->sz)
#define FIRSTCHUNK_SIZE (10)
#define BYTE_ALIGN (4)
#define NEXT_HEADER(h_p) (header_t*)((uint32_t)h_p + HSIZE + (h_p)->sz + FSIZE)
#define PAGE_SIZE (0x1000)
#define IS_ALIGNED(addr) (((addr) & 0x00000FFF) != 0)


static int is_headerp_valid(header_t *h) {
	footer_t *f = CAL_FOOTER(h);

	return 	(h->magic == ALLOC_MAGIC) && // header magic
			(f->magic == ALLOC_MAGIC) && // footer magic
			(f->header == h);			// size
}

static int is_footerp_valid(footer_t *f) {
	return 	(f->magic == ALLOC_MAGIC) && // header magic
			(f->header->magic == ALLOC_MAGIC) && // footer magic
			(f == CAL_FOOTER(f->header)); // size
}

static int is_pointer_valid(void *p) {
	p -= HSIZE;
	return is_headerp_valid(p);
}

static int is_first_seg(header_t *h) {
	return (h) == kheap->heap_base;
}

// TODO: merge block with hole
static void merge(contiguous_seg_t *seg) {
	header_t *curr_h = &seg->curr_header;
	footer_t *curr_f = CAL_FOOTER(&seg->curr_header);
	footer_t *prev_f = &seg->prev_footer;

	// are 2 both holes ?
	// is one the fisrt chunk ?
	if(!prev_f->header->is_hole || !curr_h->is_hole || is_first_seg(curr_h)) {
		return; // cannot merge blocks
	}
	
	// Merge
	prev_f->header->sz += curr_h->sz + HSIZE + FSIZE;
	curr_f->header = prev_f->header;
	
	// Checking after merge
	is_headerp_valid(prev_f->header);
	kheap->nchunks--;
}

// first fit in heap
static header_t *find(uint32_t sz, int page_align) {
	header_t *curr_h = kheap->heap_base;
	for(uint32_t i=0; i<kheap->nchunks; i++) {
		if(curr_h->is_hole) {
			if (page_align) {
				if (curr_h->sz <= sz) {
					curr_h = NEXT_HEADER(curr_h);
					continue;
				}
				uint32_t offs= (uint32_t)curr_h%PAGE_SIZE;
				uint32_t offs_left = PAGE_SIZE - offs;

				uint32_t newp = (uint32_t)curr_h + offs_left;

				uint32_t hole1h = (uint32_t) curr_h;
				int hole1sz = newp - hole1h - HSIZE;
				
				if(hole1sz < BYTE_ALIGN) {
					curr_h = NEXT_HEADER(curr_h);
					continue;
				}

				uint32_t aligned_size = offs_left +sz +FSIZE;
				if ((curr_h->sz+HSIZE+FSIZE) == aligned_size || 
						(curr_h->sz+HSIZE+FSIZE) >= aligned_size 
						+ HSIZE + BYTE_ALIGN + FSIZE 
				) {
					if(!is_headerp_valid(curr_h)) {
						PANIC("found a invalid chunk");
					} else {
						return curr_h;
					}
				}
			} else if (curr_h->sz == sz) {
				if(!is_headerp_valid(curr_h)) {
					PANIC("found a invalid chunk");
				} else {
					return curr_h;
				}
			} else if ( curr_h->sz >= 
				// Only take those holes when splits, doesn't make a small fraction
				sz+
				HSIZE + FSIZE+ 
				BYTE_ALIGN
			)	{
				if(!is_headerp_valid(curr_h)) {
					PANIC("found a invalid chunk");
				} else {
					return curr_h;
				}
			}
		}
		curr_h = NEXT_HEADER(curr_h);
	}
	return NULL;
}

/**
 * @param		addr pointer to addr of a page which is already alocated
 * @param		sz
 */
static void write_chunk(uint32_t addr, uint32_t sz, int is_hole) {
	// TODO: asserrt here
	if(sz<4) PANIC("chunk size = %d", sz);
	((header_t*)addr)->magic = ALLOC_MAGIC;
	((header_t*)addr)->sz = sz;
	((header_t*)addr)->is_hole = is_hole;

	footer_t *f =  CAL_FOOTER(addr);
	f->header = (header_t*)addr;
	f->magic = ALLOC_MAGIC;
}

static void alloc_new_pages(uint32_t old_placement, uint32_t new_placement) 
{
	for(uint32_t i=old_placement; i<new_placement; i+=PAGE_SIZE) {
		// TODO: something is wrong here, is_kernel?
		alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
	}
	// if(new_placement%/PAGE_SIZE)
	alloc_frame(get_page(new_placement, 1, kernel_directory), 0, 0);
}

#define WRITE_BLOCK(addr, sz) write_chunk(addr, sz, 0)
#define WRITE_HOLE(addr, sz) write_chunk(addr, sz, 1)
#define WRITE_RAW_BLOCK(addr, sz) WRITE_BLOCK(addr, (sz)-HSIZE-FSIZE)
#define WRITE_RAW_HOLE(addr, sz) WRITE_HOLE(addr, (sz)-HSIZE-FSIZE)

static void *alloc_new_block(uint32_t sz, int is_align) {
	uint32_t pret = 0;
	uint32_t old_placement = placement_addr;
	if(is_align && IS_ALIGNED(placement_addr)) {
		placement_addr &= 0xFFFFF000;
		uint32_t nextpage_addr = placement_addr += PAGE_SIZE;
		placement_addr += PAGE_SIZE*(sz/PAGE_SIZE);
		placement_addr += (sz%PAGE_SIZE) + FSIZE;
		
		uint32_t rawhole_sz = nextpage_addr - old_placement - HSIZE;
		uint32_t newh = nextpage_addr - HSIZE;

		alloc_new_pages(old_placement, placement_addr);

		WRITE_RAW_HOLE(old_placement, rawhole_sz);
		WRITE_BLOCK(newh, sz);
		kheap->nchunks += 2;

		pret = nextpage_addr;
	} else {
		placement_addr += sz + HSIZE + FSIZE;
		
		alloc_new_pages(old_placement, placement_addr);

		WRITE_BLOCK(old_placement, sz);
		kheap->nchunks++;

		pret = old_placement + HSIZE;
	}

	if(!is_pointer_valid((void*)pret)) {
		PANIC("new chunk setup is wrong!");
	}

	return (void*)pret;
}

static void *split(header_t *h, uint32_t newsz) {
	uint32_t oldsz = h->sz;
	footer_t *oldf  = CAL_FOOTER(h);
	h->sz = newsz;
	contiguous_seg_t *con = (contiguous_seg_t*)CAL_FOOTER(h);

	con->prev_footer.magic = ALLOC_MAGIC;
	con->prev_footer.header = h;

	con->curr_header.magic	= ALLOC_MAGIC;
	con->curr_header.is_hole = 1;
	con->curr_header.sz = oldsz - newsz;

	footer_t *nextf = CAL_FOOTER(&con->curr_header);
	nextf->header = &con->curr_header;
	nextf->magic = ALLOC_MAGIC;
	
	oldf->header = &con->curr_header;
	kheap->nchunks++;
	return (void*)((uint32_t)h+HSIZE);
}

static void *reuse_part_aligned(header_t *h, uint32_t newsz)
{
	uint32_t hole2f = (uint32_t)CAL_FOOTER(h);
	uint32_t offs = (uint32_t)h%PAGE_SIZE;
	uint32_t offs_left = PAGE_SIZE - offs;

	uint32_t newp = (uint32_t)h + offs_left;
	uint32_t newh = newp - HSIZE;
	uint32_t newf = newp + newsz;
	WRITE_BLOCK(newh, newsz);
	kheap->nchunks++;

	// TODO: merge this 2 holes to nearest block or hole if the size is < 4;
	uint32_t hole1h = (uint32_t) h;
	int hole1sz = newh - hole1h;
	if(hole1sz >=4 ) {
		WRITE_RAW_HOLE(hole1h, hole1sz);
		if(!is_headerp_valid((header_t*)hole1h)) {
			PANIC("Something is wrong with hole1 ");
		}
		kheap->nchunks++;
	} else {
		PANIC("wrong hole!");
	}
	
	uint32_t hole2h = newf + FSIZE;
	int hole2sz = hole2f - hole2h;
	if(hole2f != newf) {
		if(hole2sz >= (int)(HSIZE+BYTE_ALIGN+FSIZE)) {
			WRITE_RAW_HOLE(hole2h, hole2sz);
			if(!is_headerp_valid((header_t*)hole2h)) {
				PANIC("Something is wrong with hole2 ");
			}
			kheap->nchunks++;
		} else {
			// TODO: merge hole2 to left chunk
		}
	}

	if(!is_pointer_valid((void*)newp)) {
		PANIC("Something is wrong with the new pointer ");
	}
	return (void*) newp;
}

static void *reuse(header_t *h, uint32_t sz, int is_align) {
	if(h->sz < sz) {
		PANIC("why `h->sz` is smaller than `sz`???");
	}
	if(h->sz == sz) {
		h->is_hole = 0;
		return (void*)((uint32_t)h+HSIZE);
	}
	if (is_align) {
		return reuse_part_aligned(h, sz);
	} // else {
	// if(h->sz > sz)
	return split(h, sz);
}
// TODO: implement alloc page align here
static void *alloc(uint32_t sz, int is_align) 
{
	header_t *chunk = find(sz, is_align);
	if(chunk == NULL) {
		return alloc_new_block(sz, is_align);
	} else {
		return reuse(chunk, sz, is_align);
	}
}

uint32_t get_phys(uint32_t addr, page_directory_t *dir) 
{
	page_t *p = get_page(addr, 0, dir);
	return p->frame*PAGE_SIZE + (addr&0xfff);
}

uint32_t _kmalloc(uint32_t sz, uint32_t is_align, uint32_t *phys)
{
	if (kheap) {
		void *addr = alloc(sz, is_align);
		if (phys) {
			*phys = get_phys((uint32_t)addr, kernel_directory);
		}
		return (uint32_t) addr;
	} else {
		// https://wiki.osdev.org/James_Molloy%27s_Tutorial_Known_Bugs#Problem:_VFS_Code
		if(is_align && IS_ALIGNED(placement_addr)) { // If the address is not already page-aligned
			placement_addr &= 0xFFFFF000;
			placement_addr += PAGE_SIZE;
		}
		if (phys) {
			*phys = placement_addr;
		}
		uint32_t ret = placement_addr;
		placement_addr += sz;
		return ret;
	}
}


void kfree(void *p) {
	contiguous_seg_t *seg = p-sizeof(contiguous_seg_t);
	header_t *curr_h = &seg->curr_header;
	footer_t *prev_f = &seg->prev_footer;
	if (!is_headerp_valid(curr_h) && !is_first_seg(curr_h)) {
		PANIC("kfree(): free a invalid pointer!");
	}
	curr_h->is_hole = 1; // freeee!
	
	// Merge
	if(!is_footerp_valid(prev_f) && !is_first_seg(curr_h)) {
		PANIC("kfree(): free a invalid pointer!");
	}
	// First seg, no merge
	if(is_first_seg(curr_h)) return;

	merge(seg);
}

void kheap_init()
{
	kheap = (heap_t*)kmalloc(sizeof(heap_t));
	alloc_frame(get_page((uint32_t)kheap, 1, kernel_directory), 0, 0);
	kheap->heap_base = (void*)placement_addr;
	kheap->nchunks = 0;
}