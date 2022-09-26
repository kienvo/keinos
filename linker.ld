ENTRY(_start)
SECTIONS
{
    . = 1M;

    
    .text BLOCK(4K) : ALIGN(4K) /*Executable code*/
    {
        *(.multiboot) 
        *(.text) 
    }
    
    .rodata BLOCK(4K) : ALIGN(4K) /* Read-Only data */
    {
        *(.rodata)
    }
    
    .data BLOCK(4K) : ALIGN(4K) /* Read-Write data initialized */
    {
        *(.data)
    }
    
    .bss BLOCK(4K) : ALIGN(4K) /* Read-Write data uninitialized */
    {
        *(.COMMON)
        *(.bss)
    }
}