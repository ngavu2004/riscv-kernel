.section .text
.global _start

_start:
    la sp, user_stack_top
    call user_main

    li a7, 10
    ecall

1:
    j 1b

.section .bss
.align 4

user_stack:
    .space 1024

user_stack_top: