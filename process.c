#include "csr.h"
#include "process.h"
#include "uart.h"
#include "memory_management.h"



process_t NULL_PROCESS = {-1, 0, EXIT};
process_t process_table[PROCESS_TABLE_CAPACITY];
queue_t process_q;
static process_t *process_queue_storage[PROCESS_TABLE_CAPACITY];

void process_queue_init(void)
{
    queue_init(
        &process_q,
        process_queue_storage,
        PROCESS_TABLE_CAPACITY
    );
}

bool block_process(int pid) {
    // Find the process with the same pid
    for (int i=0; i < PROCESS_TABLE_CAPACITY; i++) {
        process_t *p = &process_table[i];

        if (p->pid == pid) {
            p->state = BLOCKED;
            return true;
        }
    }

    return false;
}

bool unblock_process(int pid) {
    // Find the process with the same pid

    process_t *p = find_process(pid);

    if (p != &NULL_PROCESS && p->state == BLOCKED) {
        p->state = READY;
        return true;
    }

    return false;
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
    while (!(queue_is_empty(&process_q))) {
        uart_putstr("The process queue is not empty.\n");
        process_t* p = queue_peek(&process_q);
        uart_putstr("Current program pid: ");
        uart_putuint64(p->pid);
        uart_putstr("\n");
        
        if (p->state == READY) {
            uart_putstr("\n");
            uart_putstr("=== Start executing program ");
            uart_putuint64(p->pid);
            uart_putstr(" ===\n");

            // Update the satp
            switch_page_table(p->root_pt);

            p->state = RUNNING;
            if (p->have_saved_context == true) {
                restore_context(p->pid); // restore context will write sepc to the sepc saved in saved_context
            } else {
                write_sepc(p->base_address); // else jump to the entry of the program
                // spp = 0 -> U-mode
                set_spp(0);
                // save the current stack pointer and execute sret
                __asm__ __volatile__("csrw sscratch, sp" ::: "memory");
                __asm__ __volatile__("sret");
            }

        } else if (p->state == BLOCKED) {
            // pop the process and put it at end of line
            queue_enqueue(&process_q, queue_dequeue(&process_q));
        } else if (p->state == EXIT) {
            queue_dequeue(&process_q); // remove the EXIT process
        } else if (p->state == RUNNING) {
            uart_putstr("Scheduler found a running process\n");
            return;
        }
    }

    if (queue_is_empty(&process_q)) {
        uart_putstr("No more processes.\n");
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