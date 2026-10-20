.section .text
.global _start
_start:
    li a0, 5
    li a7, 4
    ecall

    li a0, 2
    li a7, 10
    ecall