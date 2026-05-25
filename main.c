// This is the main kernel file
#include "csr.h"

// declare prototype for uart_putstr function
void uart_putstr(const char*);

// declare asm label
extern void trap_vector;
extern char _binary_user_bin_start[];
extern char _binary_user_bin_end[];

void kernel_main() {
    // Test: Print string
    // uart_putstr("Hello tiramisu!\n");

    // Compute user code size
    uint64_t size =
    _binary_user_bin_end -
    _binary_user_bin_start;

    // Copy binary to user address
    char* source = _binary_user_bin_start;
    char* dst = (char*)0x80100000;

    for(uint64_t i=0;i<size;i++) {
        dst[i] = source[i];
    }

    // register trap vector
    void *trap_vector_ptr = &trap_vector;
    write_mtvec((uint64_t)trap_vector_ptr);
    write_mepc((uint64_t)0x80100000);

    // clear mpp bit to switch to user mode
    clear_mpp();

    // execute mret
    __asm__ __volatile__ ("mret");

    while(1) {}
}