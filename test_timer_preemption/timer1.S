.section .text
.global _start
_start:
    li s0, 1
    li s1, 0

loop:
    addi s1, s1, 1
    li t0, 1000000
    blt s1, t0, loop

    mv a0, s0
    li a7, 1
    ecall
    addi s0, s0, 1
    li s1, 0
    j loop
