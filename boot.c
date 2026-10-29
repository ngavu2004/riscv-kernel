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
    uint64_t hart_id = read_mhartid();

    if (hart_id != MAIN_HARTID) {
        wait_for_hart(hart_id); // wait for main hart to release the current hart id
    } else {
        init_hart_table();
        enable_user_memory(USER_BASE_ADDRESS, 0x400000, 1);

        // create kernel page table
        kernel_root_pt = create_kernel_pagetable();
    }

    hart_boot();
}

void hart_boot(void) {
    // Enable pmp range here
    enable_user_memory(0x80000000, 0x1000000, 0);
    enable_user_memory(USER_BASE_ADDRESS, 0x400000, 1);
    enable_uart_memory();

    // Permit S-mode timer access on every hart.
    set_stce(1);
    enable_supervisor_time_counter();

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

    uart_putstr("\n==== Finshed booting! Jumping to the kernel! ====\n");

    // set address to kernel entry in S-mode
    write_mepc((uint64_t)&_kernel_entry);
    set_mpp(1); // set to S-mode
    write_a0(hart_id);
    __asm__ __volatile__("mret");
    return;
}