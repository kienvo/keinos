.intel_syntax noprefix


.macro ISR_noerrcode a
.global isr\a
isr\a :
	cli
	push 0
	push  \a
	jmp isr_common_stub
.endm
.macro ISR_errcode a
.global isr\a
isr\a :
	cli
	push  \a
	jmp isr_common_stub
.endm

ISR_noerrcode 0
ISR_noerrcode 1
ISR_noerrcode 2
ISR_noerrcode 3
ISR_noerrcode 4
ISR_noerrcode 5
ISR_noerrcode 6
ISR_noerrcode 7
ISR_errcode 8
ISR_noerrcode 9
ISR_errcode 10
ISR_errcode 11
ISR_errcode 12
ISR_errcode 13
ISR_errcode 14
ISR_noerrcode 15
ISR_noerrcode 16
ISR_noerrcode 17
ISR_noerrcode 18
ISR_noerrcode 19
ISR_noerrcode 20
ISR_noerrcode 21
ISR_noerrcode 22
ISR_noerrcode 23
ISR_noerrcode 24
ISR_noerrcode 25
ISR_noerrcode 26
ISR_noerrcode 27
ISR_noerrcode 28
ISR_noerrcode 29
ISR_noerrcode 30
ISR_noerrcode 31

isr_common_stub:
	
	pusha

	
	mov ax, ds              // ; Lower 16-bits of eax = ds.
	push eax 

	mov ax, 0x10  //; load the kernel data segment descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	call isr_handler

	pop eax        //; reload the original data segment descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax


	popa
	add esp, 8
	
	sti
	iret

	
	
