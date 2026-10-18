ENTRY(_entry)

PAGE_SIZE = 4096;

SECTIONS
{
    . = 0x80000000;
    __kernel_start = .;

    /* Boot code: R+X */
    .text.boot : ALIGN(PAGE_SIZE) {
        __boot_text_start = .;
        KEEP(*(.text.boot))
        . = ALIGN(PAGE_SIZE);
        __boot_text_end = .;
    }

    /* Kernel code section: R+X */
    .text : ALIGN(PAGE_SIZE) {
        __text_start = .;
        *(.text .text.*)
        . = ALIGN(PAGE_SIZE);
        __text_end = .;
    }

    /* RO daata: R--*/
    .rodata : ALIGN(PAGE_SIZE) {
        __ro_data_start = .; 
        *(.rodata .rodata.*)
        . = ALIGN(PAGE_SIZE) ;
        __ro_data_end = .;
    }

    /* Initialized writable data: RW- */
    .data : ALIGN(PAGE_SIZE) {
        __data_start = .;
        *(.data .data.*)
        *(.sdata .sdata.*)
        . = ALIGN(PAGE_SIZE);
        __data_end = .;
    }


    /* Uninitialized writable data: RW- */
    .bss : ALIGN(PAGE_SIZE) {
        __bss_start = .;
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)
        . = ALIGN(PAGE_SIZE);
        __bss_end = .;
    }

    __kernel_end = .;
}