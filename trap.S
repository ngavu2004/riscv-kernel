.section .text
.global trap_vector
trap_vector:
    call trap_handler
    mret