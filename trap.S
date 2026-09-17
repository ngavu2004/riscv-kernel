.section .text
.global trap_vector
.p2align 2
trap_vector:
    call trap_handler
    mret
spin:
    j spin