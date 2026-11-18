#include "csr.h"
#include "pmp.h"
#include "uart.h"
#include "memory_management.h"
#include "hart.h"

extern void _kernel_entry(void);
extern void mtrap_handler(void);
extern void trap_vector(void);

__attribute__((section(".text.boot")))
void boot_main(void) {
    uart_putline("The kernel has booted!\n");
    uint64_t hart_id = read_mhartid();

    
    if (hart_id != MAIN_HARTID) {
        wait_for_hart(hart_id); // wait for main hart to release the current hart id
    } else {
        init_hart_table();
        enable_user_memory(USER_BASE_ADDRESS, 0x400000, 1);
        uart_put_debug("Finished enable user memory\n");
        // create kernel page table
        kernel_root_pt = create_kernel_pagetable();
        uart_put_debug("Finished create kernel page table\n");
    }

    hart_boot();
}

void hart_boot(void) {
    // Enable pmp range here
    enable_user_memory(KERNEL_BASE, 0x1000000, 0); // since M-mode dont separate between S-mode and U-mode, we need to enable this for the S-mode to access its memory
    enable_user_memory(USER_BASE_ADDRESS, 0x400000, 1);
    enable_uart_memory();

    // Permit S-mode timer access on every hart.
    set_stce(1);
    // enable_supervisor_time_counter();
    // enable_timer_machine_interrupt();

    // switch to kernel root table
    switch_page_table(kernel_root_pt);

    // find the current hart
    uint64_t hart_id = read_mhartid();
    hart_t *hart = find_hart(hart_id);
    if (hart == NULL) {
        while (1) {
            __asm__ volatile("wfi");
        }
    }

    __asm__ volatile("mv tp, %0" : : "r"(hart) : "tp");

    // register kernel entry and trap vectors for M-mode and S-mode
    write_mtvec((uint64_t)&mtrap_handler);
    write_stvec((uint64_t)&trap_vector);

    // Keep the per-hart stack selected by boot.S.
    hart->stack_pointer = read_sp();

    uart_putstr("\r\n==== Finshed booting! Jumping to the kernel! ====\r\n");

    // set address to kernel entry in S-mode
    uart_put_debug("Writing mepc...");
    write_mepc((uint64_t)&_kernel_entry);

    uart_put_debug("Seting mpp to S-mode...");
    set_mpp(1); // set to S-mode
    uart_put_debug("Starting mret...");
    write_a0(hart_id);
    __asm__ __volatile__("mret");
    return;
}