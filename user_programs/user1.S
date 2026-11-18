.section .text
.global _start
_start:
    la a0, message
    li a7, 1
    ecall

    li a7, 10
    ecall

message:
    .asciz "Yooooo, I'm process 1!"