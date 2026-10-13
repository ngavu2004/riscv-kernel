#include "csr.h"
#include "pmp.h"
#include "uart.h"
#include "memory_management.h"

extern void _kernel_entry(void);
extern void mtrap_handler(void);
extern void trap_vector(void);

__attribute__((section(".text.boot")))
void boot_main(void) {
    // Enable pmp range here
    enable_user_memory(0x80000000, 0x400000, 0);
    enable_user_memory(USER_BASE_ADDRESS, 0x400000, 1);
    enable_uart_memory();

    // create kernel page table
    
    // register kernel entry and trap vectors for M-mode and S-mode
    write_mtvec((uint64_t)&mtrap_handler);
    write_stvec((uint64_t)&trap_vector);

    uart_putstr("\n==== Finshed booting! Jumping to the kernel! ====\n");
    
    // set address to kernel entry in S-mode
    write_mepc((uint64_t)&_kernel_entry);
    set_mpp(1); // set to S-mode
    __asm__ __volatile__("mret");
    return;
}