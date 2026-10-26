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
    if (read_mhartid() != MAIN_HARTID) {
        secondary_hart_boot();
        return;
    }

    main_hart_boot();
}

void main_hart_boot(void)
{
    init_hart_table();

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

    hart_t *hart = find_hart(MAIN_HARTID);
    uintptr_t stack_pointer;
    __asm__ volatile("mv %0, sp" : "=r"(stack_pointer));
    hart->stack_pointer = stack_pointer;

    uart_putstr("\n==== Finshed booting! Jumping to the kernel! ====\n");

    // set address to kernel entry in S-mode
    write_mepc((uint64_t)&_kernel_entry);
    set_mpp(1); // set to S-mode
    __asm__ __volatile__("mret");
    return;
}

void secondary_hart_boot(void)
{
    uint64_t hart_id = read_mhartid();
    hart_t *hart = find_hart(hart_id);

    if (hart == NULL) {
        while (1) {
            __asm__ volatile("wfi");
        }
    }

    wait_for_hart((int)hart_id);

    hart->hart_id = hart_id;
    hart->current_thread = &NULL_THREAD;
    hart->stack_pointer = 0;

    uart_putstr("Secondary hart is running in parallel.\n");

    while (1) {
        __asm__ volatile("wfi");
    }
}