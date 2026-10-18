ENTRY(_start)
PAGE_SIZE = 4096;

SECTIONS
{
    . = 0x00;

    .text : ALIGN(PAGE_SIZE) {
        *(.text*)
    }

    .rodata : ALIGN(PAGE_SIZE) {
        *(.rodata*)
    }

    .data : ALIGN(PAGE_SIZE) {
        *(.data*)
    }

    .bss : ALIGN(PAGE_SIZE) {
        *(.bss*)
        *(COMMON)
    }
}