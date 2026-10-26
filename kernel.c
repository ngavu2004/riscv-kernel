// This is the main kernel file
#include "csr.h"
#include "elf.h"
#include "process.h"
#include "pmp.h"
#include "message.h"
#include "memory_management.h"
#include "thread.h"
#include "timer.h"
#include "hart.h"

// declare prototype for uart_putstr function
void uart_putstr(const char *);

// declare asm label
extern char stack_top[];
extern unsigned char _binary_user1_elf_start[];
extern unsigned char _binary_user1_elf_end[];
extern unsigned char _binary_user2_elf_start[];
extern unsigned char _binary_user2_elf_end[];
extern unsigned char _binary_uart_user_elf_start[];
extern unsigned char _binary_uart_user_elf_end[];

#define UART_USER_PID 2

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
    unsigned char* elf_headers[] = {_binary_user2_elf_start, _binary_user1_elf_start, _binary_uart_user_elf_start};

    int count = (int) sizeof(elf_headers)/sizeof(elf_headers[0]);

    // initialize the process queue
    init_thread_queue();
    int thread_count = 0;

    uart_putstr("== Start loading ");
    uart_putuint64(count);
    uart_putstr(" program(s)==\n");

    for (int i = 0; i < count; i++) {
        uart_putstr("\n");
        uart_putstr("Program ");
        uart_putuint64((uint64_t) i);
        uart_putstr(":\n");

        // Get the process
        process_t *p = &process_table[i];
        ElfHeader *elf = (ElfHeader *)elf_headers[i];

        p->pid = i;
        p->base_address = elf->e_entry;
        p->state = PROCESS_ACTIVE;
        p->root_pt = (uintptr_t) allocate_page();

        map_kernel_address((pagetable_t *)p->root_pt);

        // if the index is the index of the UART program
        if (i == UART_USER_PID) {
            map_user_uart((pagetable_t *)p->root_pt);
        }

        // If succeed loading the elf file
        if (load_elf_header(elf, elf_headers[i], i, p->root_pt) == 0) {
            p->state = PROCESS_ACTIVE;

            // create 1 main thread for each process
            thread_t *p_main_thread = &thread_table[thread_count];

            if (!initialize_main_thread(p, p_main_thread)) {
                uart_putstr("Failed to create user thread\n");
                // TODO: Mark the process as failed and clean            }
            } else{
                // Assign an id to the thread and Put it in the queue
                p_main_thread->tid = thread_count;
                queue_enqueue(&thread_q, p_main_thread);
                thread_count++;
                uart_putstr("Enqueued thread ");
                uart_putuint64((uint64_t) p_main_thread->tid);
                uart_putstr(".\n");
            }
        }
    }

    // Set up the message queue
    initialize_message_queue(thread_count);

    uart_putstr("== Finished loading ");
    uart_putuint64(count);
    uart_putstr(" program(s)==\n");
    
    // print the queue
    for (int i = 0; i < thread_q.count; i++) {
        thread_t *thread = (thread_t *) thread_q.items[i];
        uart_putstr("TID: ");
        uart_putuint64(thread->tid);
        uart_putstr("\n");
    }

    // release the secondary hart after finish setting up shared resources
    release_hart(1);

    // init the timer
    timer_init();

    // Execute the process queue
    scheduler();
    
    while (1)
    {
    }
}