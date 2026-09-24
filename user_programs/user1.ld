ENTRY(_start)

SECTIONS
{
    . = 0x80100000;

    .text : {
        *(.text*)
    }
}