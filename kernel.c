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
#include "uart.h"

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

static void main_hart_kernel(void)
{
    // == Test: if stack smashing protector works ==
    // uart_putstr("Start SSP test..\n");
    // test_ssp();
    // uart_putstr("SSP test done");

    // 1. Load the programs
    // Note: Ideally I would want memcmp here but since this is bare metal and I cant use any C libary, we will make do with this for now
    unsigned char* elf_headers[] = {_binary_user2_elf_start, _binary_user1_elf_start, _binary_uart_user_elf_start};

    int count = (int) sizeof(elf_headers)/sizeof(elf_headers[0]);

    int thread_count = 0;
    queue_t *main_thread_queue = &find_hart(MAIN_HARTID)->thread_queue;
    queue_t *secondary_thread_queue = &find_hart(1)->thread_queue;

    uart_putstr_direct("== Start loading ");
    uart_putuint64_direct(count);
    uart_putstr_direct(" program(s)==\n");

    for (int i = 0; i < count; i++) {

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
                uart_putstr_direct("Failed to create user thread\n");
                // TODO: Mark the process as failed and clean            }
            } else{
                // Assign an id to the thread and Put it in the queue
                p_main_thread->tid = thread_count;
                queue_t *thread_queue =
                    i == UART_USER_PID ? secondary_thread_queue : main_thread_queue;
                if (i == UART_USER_PID) {
                    uart_thread_tid = p_main_thread->tid;
                }
                queue_enqueue(thread_queue, p_main_thread);
                thread_count++;
            }
        }
    }

    // Set up the message queue
    initialize_message_queue(thread_count);
    
    // release the secondary harts after finish setting up shared resources
    release_hart(1);

    // The main hart runs the user programs.
    timer_init();
    scheduler();
}

static void secondary_hart_kernel(uint64_t hart_id)
{
    queue_t *thread_queue = &find_hart(hart_id)->thread_queue;

    uart_putstr_direct("Secondary hart is running in parallel.\n");

    timer_init();

    // user uart belongs to the 2nd hart so it's not ready till the 2nd hart init
    uart_set_ready();
    scheduler();
}

void kernel_main(uint64_t hart_id)
{
    if (hart_id == MAIN_HARTID) {
        main_hart_kernel();
    } else {
        secondary_hart_kernel(hart_id);
    }

    while (1) {
    }
}