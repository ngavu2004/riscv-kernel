#include "csr.h"
#include "process.h"
#include "thread.h"
#include "uart.h"
#include "memory_management.h"
#include "context_switch.h"
#include "hart.h"



process_t NULL_PROCESS = {
    .pid = UINT64_MAX,
    .base_address = 0,
    .state = PROCESS_EXIT,
    .root_pt = 0
};
process_t process_table[PROCESS_TABLE_CAPACITY];
queue_t process_q;
static void *process_queue_storage[PROCESS_TABLE_CAPACITY];

void process_queue_init(void)
{
    queue_init(
        &process_q,
        process_queue_storage,
        PROCESS_TABLE_CAPACITY
    );
}

process_t* find_process(int pid) {
    for (int i=0; i < PROCESS_TABLE_CAPACITY;i++) {
        process_t *p = &process_table[i];

        if (p->pid == pid) {
            return p;
        }
    }
    return &NULL_PROCESS;
}

void scheduler() {
    hart_t *hart = current_hart();
    queue_t *thread_queue = current_hart_thread_queue();

    if (hart == 0 || thread_queue == 0) {
        uart_putstr("Scheduler could not find the current hart.\n");
        return;
    }

    while (!(queue_is_empty(thread_queue))) {
        thread_t* t = queue_peek(thread_queue);
        
        if (t->state == READY) {

            // Update the satp to the thread process pt
            process_t* p = find_process(t->pid);
            switch_page_table(p->root_pt);

            t->state = RUNNING;
            hart->current_thread = t;
            restore_context(t->tid);
        } else if (t->state == BLOCKED) {
            // pop the process and put it at end of line
            queue_enqueue(thread_queue, queue_dequeue(thread_queue));
        } else if (t->state == EXIT) {
            queue_dequeue(thread_queue); // remove the exited thread
        } else if (t->state == RUNNING) {
            uart_putstr_direct("Scheduler found a running process\n");
            return;
        }
    }

    if (queue_is_empty(thread_queue)) {
        hart->current_thread = &NULL_THREAD;
        uart_putstr_direct("No more active threads.\n");
        while (1) {
            __asm__ volatile("wfi");
        }
    }
}

int process_queue_curr_pid() {
    if (!queue_is_empty(&process_q)) {
        process_t * curr_p = queue_peek(&process_q);
        return curr_p->pid;
    }
    return -1;
}