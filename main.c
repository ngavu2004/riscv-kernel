// This is the main kernel file
#include "csr.h"
#include "elf.c"
#include "process.c"

// declare prototype for uart_putstr function
void uart_putstr(const char *);

// declare asm label
extern void trap_vector;
extern unsigned char _binary_user1_elf_start[];
extern unsigned char _binary_user1_elf_end[];
extern unsigned char _binary_user2_elf_start[];
extern unsigned char _binary_user2_elf_end[];

uint64_t min_int(uint64_t a, uint64_t b)
{
    return (a < b) ? a : b;
}

// stack smash protector test function
// __attribute__((noinline))
// static void test_ssp(void) {
//     volatile char buffer[8];

//     for (uint64_t i=0; i<16; i++) {
//         buffer[i] = 'a';
//     }
// }

void kernel_main()
{
    // == Test: if stack smashing protector works ==
    // uart_putstr("Start SSP test..\n");
    // test_ssp();
    // uart_putstr("SSP test done");

    // 1. Load the programs
    // Note: Ideally I would want memcmp here but since this is bare metal and I cant use any C libary, we will make do with this for now
    unsigned char* elf_headers[] = {_binary_user1_elf_start, _binary_user2_elf_start};

    int count = (int) sizeof(elf_headers)/sizeof(elf_headers[0]);

    uart_putstr("== Start loading ");
    uart_putuint64(count);
    uart_putstr(" program(s)==\n");

    for (int i=0; i < count; i++) {
        uart_putstr("\n");
        uart_putstr("Program ");
        uart_putuint64((uint64_t) i);
        uart_putstr(":\n");

        // Create a process
        process_t p;
        ElfHeader* elf = elf_headers[i];
        p.base_address = elf->e_entry;
        p.state = PENDING;

        // If succeed loading the elf file
        if (load_elf_header(elf, elf_headers[i]) == 0) {
            p.state = READY;
            
            // Put it in the queue
            enqueue(p);
            uart_putstr("Enqueued program ");
            uart_putuint64((uint64_t) i);
            uart_putstr(".\n");
        }
    }

    uart_putstr("== Finished loading ");
    uart_putuint64(count);
    uart_putstr(" program(s)==\n");

    // register trap vector
    void *trap_vector_ptr = &trap_vector;
    write_mtvec((uint64_t)trap_vector_ptr);

    // Execute the process queue

    while (!is_empty()) {
        uart_putstr("The process queue is not empty.\n");
        process_t* p = dequeue();
        
        if (p->state == READY) {
            write_mepc(p->base_address);
            // clear mpp bit to switch to user mode
            clear_mpp();

            // execute mret
            uart_putstr("Start executing the program\n");
            __asm__ __volatile__("mret");
        }
    }
    
    while (1)
    {
    }
}