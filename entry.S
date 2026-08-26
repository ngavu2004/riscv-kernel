.section .text
.global _entry
_entry:
        # load address of stack_top to sp
        la sp, stack_top
        
        # jump into kernel code 
        call kernel_main

spin:
        j spin

.section .bss
.align 12

stack:
    .space 4096
stack_top: