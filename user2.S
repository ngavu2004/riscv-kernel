.section .text
.global _start
_start:
    li a0, 45
    li a7, 1
    ecall
spin:
    j spin