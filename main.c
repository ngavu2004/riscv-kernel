// This is the main kernel file
#include "csr.h"
#include "elf_helper.h"

// declare prototype for uart_putstr function
void uart_putstr(const char*);

// declare asm label
extern void trap_vector;
extern unsigned char _binary_user_elf_start[];
extern unsigned char _binary_user_elf_end[];

uint64_t min_int(uint64_t a, uint64_t b) {
    return (a < b) ? a : b;
}


void kernel_main() {
    // Test: Print string
    uart_putstr("Hello tiramisu!\n");
    uart_putstr("Binary user elf start: ");
    uart_puthex((uint64_t) _binary_user_elf_start);
    uart_putstr("\n");
    // 1. Verify the first few bytes are actually elf file
    // Note: Ideally I would want memcmp here but since this is bare metal and I cant use any C libary, we will make do with this for now
    ElfHeader* elf = _binary_user_elf_start;
   if (elf->e_ident[0] == 0x7f && elf->e_ident[1] == 'E' && elf->e_ident[2] == 'L' && elf->e_ident[3] == 'F') {
        uart_putstr("Valid ELF file!\n");

        // 2. Parse and handle the program header
            // Loop through e_phnum -> find the one with PT_Load segment type
        for (int i = 0; i < elf->e_phnum; i++) {
            ProgramHeader *curr_p = (ProgramHeader*)((char*)_binary_user_elf_start + elf->e_phoff + i * elf->e_phentsize);
            if (curr_p->p_type == PT_LOAD) {
                uart_putstr("Load segment: ");
                uart_puthex(curr_p->p_vaddr);
                uart_putstr("\n");
                
                enable_user_memory(curr_p->p_vaddr, curr_p->p_memsz);

                // Copy p_filesz bytes from p_offset to p_vaddr
                char* src_addr =  (char*)_binary_user_elf_start + curr_p->p_offset;
                char* dst_addr = (char *)(uintptr_t)curr_p->p_vaddr;
                for (uint64_t j=0; j < curr_p->p_filesz; j++) {
                    dst_addr[j] = src_addr[j];
                };

                // Zero out the bytes from p_filesz to p_memsz
                for (uint64_t j=curr_p->p_filesz; j < curr_p->p_memsz; j++) {
                    dst_addr[j] = 0;
                }
            };
        }

        uart_putstr("Finished copy the load segments\n");

        // register trap vector
        void *trap_vector_ptr = &trap_vector;
        write_mtvec((uint64_t)trap_vector_ptr);
        uart_putstr("user entry: "); 
        uart_puthex(elf->e_entry); 
        uart_putstr("\n");
        uart_putstr("user end: "); 
        uart_puthex((uint64_t)_binary_user_elf_end); 
        uart_putstr("\n");
        write_mepc(elf->e_entry);

        // clear mpp bit to switch to user mode
        clear_mpp();
        uart_putstr("Done\n");

        // execute mret
        __asm__ __volatile__ ("mret");
    } else {
        uart_putstr("Invalid ELF header. Exit.");
        
    }
    
    while(1) {}
}