#include "csr.h"
#include "process.h"
#include "thread.h"
#include "uart.h"
#include "memory_management.h"
#include "context_switch.h"



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
    while (!(queue_is_empty(&thread_q))) {
        uart_putstr("The thread queue is not empty.\n");
        thread_t* t = queue_peek(&thread_q);

        uart_putstr("Current program tid: ");
        uart_putuint64(t->tid);
        uart_putstr("\n");
        
        if (t->state == READY) {
            uart_putstr("\n");
            uart_putstr("=== Start executing thread ");
            uart_putuint64(t->tid);
            uart_putstr(" ===\n");

            // Update the satp to the thread process pt
            process_t* p = find_process(t->pid);
            switch_page_table(p->root_pt);

            t->state = RUNNING;
            restore_context(t->tid);
        } else if (t->state == BLOCKED) {
            // pop the process and put it at end of line
            queue_enqueue(&thread_q, queue_dequeue(&thread_q));
        } else if (t->state == EXIT) {
            queue_dequeue(&thread_q); // remove the exited thread
        } else if (t->state == RUNNING) {
            uart_putstr("Scheduler found a running process\n");
            return;
        }
    }

    if (queue_is_empty(&thread_q)) {
        uart_putstr("No more active threads.\n");
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