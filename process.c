#include "csr.h"
#include "process.h"
#include "uart.h"

process_t NULL_PROCESS = {-1, 0, EXIT};

process_queue_t process_q = {
    .front = -1,
    .back = 0
};

bool block_process(int pid) {
    // Find the process with the same pid
    for (int i=0; i < process_q.back; i++) {
        process_t *p = &process_q.queue[i];
        if (p->pid == pid) {
            p->state = BLOCKED;
            return true;
        }
    }

    return false;
}

bool unblock_process(int pid) {
    // Find the process with the same pid
    for (int i=0; i < process_q.back; i++) {
        process_t *p = &process_q.queue[i];
        if (p->pid == pid && p->state == BLOCKED ) {
            p->state = READY;
            return true;
        }
    }

    return false;
}


int enqueue(process_t p) {
    // If the queue is already full
    if (process_queue_is_full()) {
        return -1;
    }

    process_q.queue[process_q.back] = p;
    process_q.back++;
    return process_q.back;
}

process_t* front() {
    if (!process_queue_is_empty()) {
        process_t* p = &process_q.queue[0];
        return p;
    }
    return &NULL_PROCESS;
}

process_t* dequeue() {
    if (!process_queue_is_empty()) {
        process_t* p = &process_q.queue[0];
        // Move other processes up
        if (process_q.back > 1) {
            for (int i = 1; i < process_q.back; i++) {
                process_q.queue[i-1] = process_q.queue[i];
            }
        }
        process_q.back --;
        return p;
    } else {
        return &NULL_PROCESS;
    };
}

process_t* find_process(int pid) {
    for (int i=0; i<process_q.back;i++) {
        process_t *p = &process_q.queue[i];
        if (p->pid == pid) {
            return p;
        }
    }
    return &NULL_PROCESS;
}

void execute_processes() {
    while (!process_queue_is_empty()) {
        uart_putstr("The process queue is not empty.\n");
        process_t* p = front();
        uart_putstr("Current program pid: ");
        uart_putuint64(p->pid);
        uart_putstr("\n");
        
        if (p->state == READY) {
            uart_putstr("\n");
            uart_putstr("=== Start executing program ");
            uart_putuint64(p->pid);
            uart_putstr(" ===\n");
            p->state = RUNNING;
            if (p->have_saved_context == true) {
                restore_context(p->pid); // restore context will write sepc to the sepc saved in saved_context
            } else {
                write_sepc(p->base_address); // else jump to the entry of the program
                // mpp = 0 -> U-mode
                set_spp(0);
                // execute sret
                __asm__ __volatile__("sret");
            }

        } else if (p->state == BLOCKED) {
            // pop the process and put it at end of line
            enqueue(*dequeue());
        } else if (p->state == EXIT) {
            dequeue(); // remove the EXIT process
        }
    }

    if (process_queue_is_empty()) {
        uart_putstr("No more processes.\n");
        while (1) {
            __asm__ volatile("wfi");
        }
    }
}

int process_queue_curr_pid() {
    if (!process_queue_is_empty()) {
        return process_q.queue[process_q.front+1].pid;
    }
    return -1;
}

bool process_queue_is_empty() {
    return (process_q.back == 0);
}

bool process_queue_is_full() {
    return process_q.back == USER_PROCESS_SLOT_NUM;
}