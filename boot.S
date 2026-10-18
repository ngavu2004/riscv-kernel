.section .text.boot
.global _entry
_entry: 
        # The boot stack must remain in the physical boot image.
        la sp, boot_stack_top

        # delegate standard exceptions to S-mode.
        li      t0, 0xffff
        csrw medeleg, t0
        
        # jump into boot entry
        call boot_main

spin:
        j spin

.section .bss.boot
.align 12

boot_stack:
    .space 4096
boot_stack_top: