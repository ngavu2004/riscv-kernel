.section .text.kernel
.global _kernel_entry
_kernel_entry:
        
        # jump into boot entry
        call kernel_main

spin:
        j spin