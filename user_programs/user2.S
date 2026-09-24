.section .text
.global _start
_start:
    la a0, message
    li a1, 0
    li a7, 2
    ecall

    la a0, message
    li a1, 1
    li a7, 2
    ecall

    li a7, 10
    ecall

message:
    .asciz "Hello, from process 2!"