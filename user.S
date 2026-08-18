.section .text
.global _start
_start:
    li a0, 'H'
    li t0, 97
    csrw mcause, t0
    ecall
spin:
    j spin