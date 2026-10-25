.section .text
.global _start
_start:
    la a0, message
    li a1, 2
    li a7, 2
    ecall

    la a0, message
    li a1, 0
    li a7, 3
    ecall

    li a7, 10
    ecall

message:
    .asciz "Hello, from process 2!"