.section .text.boot
.global _entry
_entry: 
        # Select a private boot stack for this hart.
        csrr t0, mhartid
        la   sp, boot_stacks
        li   t1, 4096
        addi t0, t0, 1
        mul  t0, t0, t1
        add  sp, sp, t0

        # delegate standard exceptions to S-mode.
        li      t0, 0xffff
        csrw medeleg, t0

        # Delegate supervisor timer interrupts to S-mode.
        li      t0, (1 << 5)
        csrs    mideleg, t0
        
        # jump into boot entry
        call boot_main

spin:
        j spin

.section .bss.boot
.align 12

boot_stacks:
    .space 4096 * 2
boot_stack_tops: