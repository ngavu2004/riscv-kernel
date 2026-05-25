.section .text
.global _start
_start:
    li a0, 'H'
    ecall
spin:
    j spin