// This is the main kernel file
#include "csr.h"

// declare prototype for uart_putstr function
void uart_putstr(const char*);

// declare asm label
extern void trap_vector;
extern char _binary_user_elf_start[];
extern char _binary_user_elf_end[];

void kernel_main() {
    // Test: Print string
    // uart_putstr("Hello tiramisu!\n");

    // 1. Verify the first few bytes are actually elf file
    char *elf = _binary_user_elf_start;
    if (elf[0] == 0x7f  && elf[1] == 'E' && elf[2] == 'L' && elf[3] == 'F') {
        uart_putstr("Valid ELF file");

        // 2. Parse elf header
    

        // 3. Load the PT_Load

        // allow U-mode to fetch and access the loaded image
        enable_user_memory();

        // register trap vector
        void *trap_vector_ptr = &trap_vector;
        write_mtvec((uint64_t)trap_vector_ptr);
        write_mepc((uint64_t)0x80100000);

        // clear mpp bit to switch to user mode
        clear_mpp();

        // execute mret
        __asm__ __volatile__ ("mret");
    } else {
        uart_putstr("Invalid ELF header. Exit.");
        
    }
    
    while(1) {}
}