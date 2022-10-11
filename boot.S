.att_syntax prefix
.set ALIGN,     1<<0
.set MEMINFO,    1<<1
.set FLAGS,     ALIGN | MEMINFO
.set MAGIC,     0x1BADB002
.set CHECKSUM,  -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM 

.section .bss
.align 16
stack_bottom:
.skip 0x4000 // 16KiB
stack_top: 

.section .text
.global _start
.type _start, @function
_start:
    mov $stack_top, %esp
	/* Reset EFLAGS. */
	pushl   $0
	popf

	/* Push the pointer to the Multiboot information structure. */
	push  	%ebx
	/* Push the magic value. */
	push 	%eax

    call kernel_main
.global hang
hang:
    cli
    hlt
    jmp hang
.size _start, . - _start
