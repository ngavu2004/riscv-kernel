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
extern unsigned char _binary_user1_elf_start[] __attribute__((weak));
extern unsigned char _binary_user1_elf_end[] __attribute__((weak));
extern unsigned char _binary_user2_elf_start[] __attribute__((weak));
extern unsigned char _binary_user2_elf_end[] __attribute__((weak));
extern unsigned char _binary_uart_user_elf_start[] __attribute__((weak));
extern unsigned char _binary_uart_user_elf_end[] __attribute__((weak));
extern unsigned char _binary_monitor_elf_start[] __attribute__((weak));
extern unsigned char _binary_monitor_elf_end[] __attribute__((weak));
extern unsigned char _binary_observed_program_elf_start[] __attribute__((weak));
extern unsigned char _binary_observed_program_elf_end[] __attribute__((weak));


#define UART_USER_PID 2

uint64_t min_int(uint64_t a, uint64_t b)
{
    return (a < b) ? a : b;
}

typedef struct {
    unsigned char *elf_start;
    uint64_t hart_id;
} elf_program_t;

typedef struct {
    process_t *uart_program;
    process_t *monitor_program;
    process_t *observed_program;
} special_program_t;

void setup_special_program(special_program_t *special_program_pointer, char * elf_start, process_t* p) {
    if (elf_start != 0) {
        if (elf_start == _binary_uart_user_elf_start)  {
            special_program_pointer->uart_program = p;
        } else if (elf_start == _binary_monitor_elf_start) {
            special_program_pointer->monitor_program = p;
        } else if (elf_start == _binary_observed_program_elf_start) {
            special_program_pointer->observed_program = p;
        }
    }
}

static void main_hart_kernel(void)
{
    // == Test: if stack smashing protector works ==
    // uart_putstr("Start SSP test..\n");
    // test_ssp();
    // uart_putstr("SSP test done");

    // 1. Load the programs
    // Note: Ideally I would want memcmp here but since this is bare metal and I cant use any C libary, we will make do with this for now
    // UPDATE here depends on the program you want to run
    elf_program_t programs[] = {
        {_binary_observed_program_elf_start, MAIN_HARTID},
        {_binary_monitor_elf_start, 1}
    };

    special_program_t special_program_pointer = {0, 0};

    int count = sizeof(programs) / sizeof(programs[0]);

    int thread_count = 0;
    queue_t *main_thread_queue = &find_hart(MAIN_HARTID)->thread_queue;
    queue_t *secondary_thread_queue = &find_hart(1)->thread_queue;

    uart_putstr_direct("== Start loading ");
    uart_putuint64_direct(count);
    uart_putstr_direct(" program(s)==\n");

    for (int i = 0; i < count; i++) {
        elf_program_t *program = &programs[i];

        queue_t *thread_queue = program->hart_id == MAIN_HARTID ? main_thread_queue : secondary_thread_queue;

        // Get the process
        process_t *p = &process_table[i];
        ElfHeader *elf = (ElfHeader *)program->elf_start;

        p->pid = i;
        p->base_address = elf->e_entry;
        p->state = PROCESS_ACTIVE;
        p->root_pt = (uintptr_t) allocate_page();

        map_kernel_address((pagetable_t *)p->root_pt);

        // if the index is the index of the UART program
        if (p->pid == UART_USER_PID) {
            map_user_uart((pagetable_t *)p->root_pt);
        }

        // If succeed loading the elf file
        if (load_elf_header(elf, program->elf_start, i, p->root_pt) == 0) {
            p->state = PROCESS_ACTIVE;
            setup_special_program(&special_program_pointer, program->elf_start, p);

            // create 1 main thread for each process
            thread_t *p_main_thread = &thread_table[thread_count];

            if (!initialize_main_thread(p, p_main_thread)) {
                uart_putstr_direct("Failed to create user thread\n");
                // TODO: Mark the process as failed and clean            }
            } else{
                // Assign an id to the thread and Put it in the queue
                p_main_thread->tid = thread_count;

                if (p->pid == UART_USER_PID) {
                    uart_thread_tid = p_main_thread->tid;
                }
                
                if (queue_enqueue(thread_queue, p_main_thread)) {
                    thread_count++;
                } else {
                    uart_putstr_direct("Failed to enqueue user thread\n");
                }
            }
        }
    }

    // Set up the message queue
    initialize_message_queue(thread_count);

    // if the monitor is present, map a shared page between it and the observed program
    if (special_program_pointer.monitor_program != 0 && special_program_pointer.observed_program != 0) {
        // allocate a page as a shared page between the monitor and the observed program
        void *page = allocate_page();

        if (page == NULL) {
            uart_putstr_direct("Failed to allocate shared page\n");
        } else {
            // map the allocated page to MONITOR VA in the observed program
            map_page(special_program_pointer.observed_program->root_pt, MONITOR_VA, page, 1, PTE_V | PTE_R | PTE_W | PTE_U);

            // map the page to monitor program as read only
            map_page(special_program_pointer.monitor_program->root_pt, MONITOR_VA, page, 1, PTE_V | PTE_R | PTE_U);
        }
        
    }
    
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
    if (uart_defined()) {
        uart_set_ready();
    }
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