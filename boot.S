.section .text.boot
.global _entry
_entry: 
        # load address of stack_top to sp
        la sp, stack_top

        # delegate u-mode ecall to s-mode
        li t0, (1 << 8)           # Delegate U-mode ecalls
        csrw medeleg, t0
        
        # jump into boot entry
        call boot_main

spin:
        j spin

.section .bss
.align 12

stack:
    .space 4096
stack_top: