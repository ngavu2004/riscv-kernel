.section .text
.global _start
_start:
    li a0, 42
    li a7, 1
    ecall
spin:
    j spin