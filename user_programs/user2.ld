ENTRY(_start)
PAGE_SIZE = 4096;

SECTIONS
{
    . = 0x00;

    .text : ALIGN(PAGE_SIZE) {
        *(.text*)
    }
}