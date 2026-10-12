ENTRY(_start)

SECTIONS
{
    . = 0x80500000;

    .text : {
        *(.text*)
    }
}