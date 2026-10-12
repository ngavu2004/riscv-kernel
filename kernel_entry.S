.section .text.kernel
.global _kernel_entry
_kernel_entry:
        # load address of stack_top to sp
        la sp, stack_top
        
        # jump into boot entry
        call kernel_main

spin:
        j spin

.section .bss
.align 12

stack:
    .space 4096
stack_top: