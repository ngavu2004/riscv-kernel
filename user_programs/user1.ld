ENTRY(_start)
PAGE_SIZE = 4096;

SECTIONS
{
    . = 0x00000000;

    .text : ALIGN(4096) {
        *(.text*)
    }
}