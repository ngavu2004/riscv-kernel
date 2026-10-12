ENTRY(_start)

SECTIONS
{
    . = 0x80400000;

    .text : {
        *(.text*)
    }
}