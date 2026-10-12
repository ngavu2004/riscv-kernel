ENTRY(_entry)

SECTIONS
{
    . = 0x80000000;

    .text.boot : {
        KEEP(*(.text.boot))
    }

    .text : {
        *(.text .text.*)
    }

    .bss : {
        *(.bss .bss.*)
    }

    .data : {
        *(.data .data.*)
    }

    .rodata : { 
        *(.rodata .rodata.*) 
    }

    .sdata : {
        *(.sdata .sdata.*)
    }

    .sbss : {
        *(.sbss .sbss.*)
    }
}