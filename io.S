.intel_syntax noprefix
.section .text
    .global _outb
    
    .type outb, @function
    _outb:
        mov al, [esp + 8]
        mov dx, [esp + 4]
        out dx, al
        ret
    
    .type inb, @function
    .global _inb
    _inb:
        mov dx, [esp + 4]
        in  al,dx
        ret