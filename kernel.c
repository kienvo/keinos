#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "io.h"
#include "console.h"
#include "tty.h"
#include "common.h"
#include "printk.h"
#include "panic.h"
#include "multiboot.h"
#include "string.h"
#include "mm.h"

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler"
#endif

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))
#define MULTIBOOT_LOG(...) printk("multiboot: "__VA_ARGS__);

void get_multiboot_info(unsigned int magic, unsigned int multiboot_addr) {

  /* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		panic("Invalid magic number: 0x%X\n", (unsigned) magic);
	}

  /* Set MBI to the address of the Multiboot information structure. */
	multiboot_info_t *mbi = (multiboot_info_t *) multiboot_addr;

  /* Print out the flags. */
	MULTIBOOT_LOG ("flags = 0x%X\n", (unsigned) mbi->flags);

  /* Are mem_* valid? */
	if (CHECK_FLAG (mbi->flags, 0)) 
		MULTIBOOT_LOG ("mem_lower = %dKB, mem_upper = %dKB\n",
            (unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);

  /* Is boot_device valid? */
	if (CHECK_FLAG (mbi->flags, 1))
		MULTIBOOT_LOG ("boot_device = 0x%X\n", (unsigned) mbi->boot_device);

  /* Is the command line passed? */
	if (CHECK_FLAG (mbi->flags, 2))
		MULTIBOOT_LOG ("cmdline = '%s'\n", (char *) mbi->cmdline);

  /* Are mods_* valid? */
	if (CHECK_FLAG (mbi->flags, 3)) {
		multiboot_module_t *mod;
		uint32_t i;
		
		MULTIBOOT_LOG ("mods_count = %d, mods_addr = 0x%X\n",
				(int) mbi->mods_count, (int) mbi->mods_addr);
		for (i = 0, mod = (multiboot_module_t *) mbi->mods_addr;
			i < mbi->mods_count;
			i++, mod++)
		MULTIBOOT_LOG (" mod_start = 0x%X, mod_end = 0x%X, cmdline = %s\n",
				(unsigned) mod->mod_start,
				(unsigned) mod->mod_end,
				(char *) mod->cmdline);
    }

	/* Bits 4 and 5 are mutually exclusive! */
	if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5)) {
		panic ("multiboot: Both bits 4 and 5 are set.\n");
	}

	/* Is the symbol table of a.out valid? */
	if (CHECK_FLAG (mbi->flags, 4)) {
		multiboot_aout_symbol_table_t *multiboot_aout_sym = &(mbi->u.aout_sym);
		
		MULTIBOOT_LOG ("multiboot_aout_symbol_table: tabsize = 0x%0x, "
				"strsize = 0x%X, addr = 0x%X\n",
				(unsigned) multiboot_aout_sym->tabsize,
				(unsigned) multiboot_aout_sym->strsize,
				(unsigned) multiboot_aout_sym->addr);
	}

	/* Is the section header table of ELF valid? */
	if (CHECK_FLAG (mbi->flags, 5)) {
		multiboot_elf_section_header_table_t *multiboot_elf_sec = &(mbi->u.elf_sec);

		MULTIBOOT_LOG ("multiboot_elf_sec: num = %d, size = 0x%X,"
				" addr = 0x%X, shndx = 0x%X\n",
				(unsigned) multiboot_elf_sec->num, (unsigned) multiboot_elf_sec->size,
				(unsigned) multiboot_elf_sec->addr, (unsigned) multiboot_elf_sec->shndx);
	}

	/* Are mmap_* valid? */
	if (CHECK_FLAG (mbi->flags, 6)) {
		multiboot_memory_map_t *mmap;
		
		MULTIBOOT_LOG ("mmap_addr = 0x%X, mmap_length = 0x%X\n",
				(unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
		
		for (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
			(unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
			mmap = (multiboot_memory_map_t *) ((unsigned long) mmap
									+ mmap->size + sizeof (mmap->size)))
			printk ("\tsize = 0x%X, base_addr = 0x%X%08x,"
					" length = 0x%X%08x, type = 0x%X\n",
					(unsigned) mmap->size,
					(unsigned) (mmap->addr >> 32),
					(unsigned) (mmap->addr & 0xffffffff),
					(unsigned) (mmap->len >> 32),
					(unsigned) (mmap->len & 0xffffffff),
					(unsigned) mmap->type);
	}
}

static const char *get_cmdline(unsigned int multiboot_addr)
{
	multiboot_info_t *mbi = (multiboot_info_t *) multiboot_addr;
	return (const char*) mbi->cmdline;
}

static void parse_tty_dev(const char *cmd)
{
	if(!strncmp(cmd, "/dev/", 5)) {
		cmd += 5;
		if(!strncmp(cmd, "ttyS0", 5)) {
			set_default_tty(TTYS0);
		}
		else if(!strncmp(cmd, "ttyS1", 5)) {
			set_default_tty(TTYS1);
		}
		else if(!strncmp(cmd, "tty1", 4)) {
			set_default_tty(TTY1);
		}
	}
}

static void parse_options(const char *cmd)
{
	while(1) {
		if(!strncmp(cmd, "console=", 8)) {
			parse_tty_dev(cmd+8);
		}
		const char *next = strchr(cmd, ' ');
		if(next == NULL) return;
		cmd = ++next;
	}
}

void kernel_main(unsigned int magic, unsigned int multiboot_addr)
{
    GDT_init();
    IDT_init();
	pic_init();
    
	con_init();
	rs_init();
	parse_options(get_cmdline(multiboot_addr));
	get_multiboot_info(magic, multiboot_addr);

	sti();
	paging_init(0x1000000); // 16MiB
	rs_puts("This text is from RS puts\n", COM1);
	tty_write(0, "This text is from \nchannel 0, or COM1\n", 39);
	printk("This is from prink() %c %d 0x%X 0x%04X %s\n", 'a',-1, 0xaa, 0xaf, "string test");

//    uint32_t *ptr = (uint32_t*)0xA0000000;
//    uint32_t do_page_fault = *ptr;
	uint32_t a = kmalloc(8);
	uint32_t b = kmalloc(8);
	uint32_t c = kmalloc(8);
	kfree((void*)a);
	uint32_t d = kmalloc(8); // d should = a
	kfree((void*)b);
	kfree((void*)c);
	uint32_t f = kmalloc(12); // f should = b
	uint32_t e = kmalloc(4);  // e should reuse, not new allocated
	uint32_t block = kmalloc_a(0x1000*2); // block should aligned
	kfree((void*)block);
	uint32_t block2 = kmalloc_a(0x1000*2); // block2 should = block
	kfree((void*)block2);
	uint32_t g = kmalloc(4);  // e should reuse, not new allocated
	uint32_t block3 = kmalloc(0x1000*2); // block3 should < block2
	while(1)
    {
        __asm__ volatile("hlt");
    }
}