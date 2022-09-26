.intel_syntax noprefix
.section .text
    .global outb
    
    .type outb, @function
    outb:
        mov al, [esp + 8]
        mov dx, [esp + 4]
        out dx, al
        ret
    
    .type inb, @function
    .global inb
    inb:
        mov dx, [esp + 4]
        in  al,dx
        ret