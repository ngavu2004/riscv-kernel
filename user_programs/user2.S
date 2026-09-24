.section .text
.global _start
_start:
    la a0, message
    li a1, 0
    li a7, 2
    ecall

    li a7, 10
    ecall

message:
    .asciz "Hello, process 1!"