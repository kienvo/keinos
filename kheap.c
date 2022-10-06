#include "mm.h"
#include "list.h"
#include "panic.h"

extern page_directory_t *kernel_directory;
uint32_t placement_addr = (uint32_t)&__bss_end;
heap_t *kheap = NULL;

// Calculate footer pointer from header pointer
#define CAL_FOOTER(h_p) ((uint32_t)h_p + sizeof(header_t) + (h_p)->sz)
#define FIRSTCHUNK_SIZE (10)
#define BYTE_ALIGN (4)
#define NEXT_HEADER(h_p) ((uint32_t)h_p + sizeof(header_t) + (h_p)->sz + sizeof(footer_t))


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
	p -= sizeof(header_t);
	return is_headerp_valid(p);
}

static int is_first_seg(header_t *h) {
	return (h) == kheap->heap_base;
}

static void merge(contiguous_seg_t *seg) {
	header_t *curr_h = &seg->curr_header;
	footer_t *curr_f = CAL_FOOTER(&seg->curr_header);
	footer_t *prev_f = &seg->prev_footer;

	// are 2 both holes ?
	// is one the fisrt chunk ?
	if(!prev_f->header->is_hole || !curr_h->is_hole || is_first_seg(curr_h)) {
		return; // cannont merge blocks
	}
	
	// Merge
	prev_f->header->sz += curr_h->sz + sizeof(header_t) + sizeof(footer_t);
	curr_f->header = prev_f->header;
	
	// Checking after merge
	is_headerp_valid(prev_f->header);
	kheap->nchunks--;
}

// first fit in heap
static header_t *find(uint32_t sz) {
	header_t *curr_h = kheap->heap_base;
	for(uint32_t i=0; i<kheap->nchunks; i++) {
		if(curr_h->is_hole) {
			if(curr_h->sz == sz) {
				if(!is_headerp_valid(curr_h)) {
					PANIC("found a invalid chunk");
				} else {
					return curr_h;
				}
			} else if ( curr_h->sz >= 
				sz+
				sizeof(header_t) + sizeof(footer_t)+ 
				BYTE_ALIGN
			)	{
				// Only take those holes when splits, doesn't make a small fraction
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

static void *alloc_new_block(uint32_t sz) {
	uint32_t old_placement = placement_addr;
	placement_addr += sz + sizeof(header_t) + sizeof(footer_t);
	
	for(uint32_t i=old_placement; i<placement_addr; i+=0x1000) {
		// TODO: something is wrong here, is_kernel?
		alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
	}

	header_t *h = old_placement;
	h->magic = ALLOC_MAGIC;
	h->is_hole = 0;
	h->sz = sz;

	footer_t *f = CAL_FOOTER(h);
	f->magic = ALLOC_MAGIC;
	f->header = h;

	kheap->nchunks++;

	void *pret = old_placement + sizeof(header_t);

	if(!is_pointer_valid(pret)) {
		PANIC("new chunk setup is wrong!");
	}

	return pret;
}

static void *split(header_t *h, uint32_t newsz) {
	uint32_t oldsz = h->sz;
	footer_t *oldf  = CAL_FOOTER(h);
	h->sz = newsz;
	contiguous_seg_t *con = CAL_FOOTER(h);

	con->prev_footer.magic = ALLOC_MAGIC;
	con->prev_footer.header = h;

	con->curr_header.magic	= ALLOC_MAGIC;
	con->curr_header.is_hole = 1;
	con->curr_header.sz = newsz - oldsz;

	footer_t *nextf = CAL_FOOTER(&con->curr_header);
	nextf->header = &con->curr_header;
	nextf->magic = ALLOC_MAGIC;
	
	oldf->header = &con->curr_header;
	kheap->nchunks++;
	return (void*)((uint32_t)h+sizeof(header_t));
}

static void *reuse(header_t *h, uint32_t sz) {
	if(h->sz < sz) {
		PANIC("why `h->sz` is smaller than `sz`???");
	}
	if(h->sz == sz) {
		h->is_hole = 0;
		return (void*)((uint32_t)h+sizeof(header_t));
	}
	// if(h->sz > sz)
	return split(h, sz);
}
// TODO: implement alloc page align here
void *alloc(uint32_t sz, int is_align) 
{
	header_t *chunk = find(sz);
	if(chunk == NULL) {
		return alloc_new_block(sz);
	} else {
		return reuse(chunk, sz);
	}
}

uint32_t get_phys(uint32_t addr, page_directory_t *dir) 
{
	page_t *p = get_page(addr, 0, dir);
	return p->frame*0x1000 + addr&0xfff;
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
	kheap = kmalloc(sizeof(heap_t));
	alloc_frame(get_page((uint32_t)kheap, 1, kernel_directory), 0, 0);
	kheap->heap_base = placement_addr;
	kheap->nchunks = 0;
}