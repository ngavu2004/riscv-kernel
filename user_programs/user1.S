.section .bss
buffer:
    .space 128

.section .text
.global _start
_start:
    li a7, 10
    ecall