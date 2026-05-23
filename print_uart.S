.section .text
.global user_start
user_start:
    li a0, 8
    ecall
spin:
    j spin