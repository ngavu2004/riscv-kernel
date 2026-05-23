// This is the main kernel file
#include "csr.h"

// declare prototype for uart_putstr function
void uart_putstr(const char*);

// declare asm label
extern void trap_vector(void);
extern void user_start(void);

void kernel_main() {
    // Test: Print string
    // uart_putstr("Hello tiramisu!\n");

    // register trap vector
    write_mtvec((uint64_t)trap_vector);
    write_mepc((uint64_t)user_start);

    // clear mpp bit to switch to user mode
    clear_mpp();

    // execute mret
    __asm__ __volatile__ ("mret");

    while(1) {}
}