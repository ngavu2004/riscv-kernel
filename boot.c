#include "csr.h"
#include "pmp.h"
#include "uart.h"
#include "memory_management.h"

extern void _kernel_entry(void);
extern void mtrap_handler(void);
extern void trap_vector(void);

__attribute__((section(".text.boot")))
void boot_main(void) {
    // mcounteren.TM = 1       // permits S-mode to read time
    // menvcfg.STCE = 1       // permits S-mode stimecmp access
    set_stce(1);
    enable_supervisor_time_counter();

    // Enable pmp range here
    enable_user_memory(0x80000000, 0x1000000, 0);
    enable_user_memory(USER_BASE_ADDRESS, 0x400000, 1);
    enable_uart_memory();

    // create kernel page table
    kernel_root_pt = create_kernel_pagetable();

    uint64_t satp = (8ULL << 60) | (kernel_root_pt >> 12);  // Sv39
    __asm__ volatile( "csrw satp, %0" : : "r"(satp) : "memory");
    __asm__ volatile("sfence.vma" ::: "memory");

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