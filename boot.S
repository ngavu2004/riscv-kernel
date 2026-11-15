.option norvc
.extern mtrap_handler
.section .text.boot
.global _entry
_entry: 
        # load the UART address
        li      t3, UART0
        
        # Select a private boot stack for this har
        csrr t0, mhartid
        la   sp, boot_stacks
        li   t1, 4096
        addi t0, t0, 1
        mul  t0, t0, t1
        add  sp, sp, t0
        li      t4, 'a'
        sb      t4, 0(t3)

        # register mtrap handler
        la      t0, mtrap_handler
        csrw    mtvec, t0
        li      t4, 'b'
        sb      t4, 0(t3)
        
        # delegate standard exceptions to S-mode.
        li      t0, 0xffff
        csrw medeleg, t0
        li      t4, 'c'
        sb      t4, 0(t3)

        # Delegate supervisor timer interrupts to S-mode.
        li      t0, (1 << 5)
        csrs    mideleg, t0
        li      t0, 'd'
        sb      t0, 0(t3)
        
        # jump into boot entry
        call boot_main

spin:
        j spin

.section .bss.boot
.align 12

boot_stacks:
    .space 4096 * 2
boot_stack_tops: