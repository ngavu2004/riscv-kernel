.section .bss
buffer:
    .space 128

.section .text
.global _start
_start:
    la a0, buffer
    li a7, 3
    ecall

    li a7, 10
    ecall