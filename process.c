#include "process.h"
#include "csr.h"
#include "uart.h"

process_t NULL_PROCESS = {-1, 0, EXIT};

process_queue_t process_q = {
    .front = -1,
    .back = 0
};

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

void execute_processes() {
    while (!process_queue_is_empty()) {
        uart_putstr("The process queue is not empty.\n");
        process_t* p = front();
        uart_putstr("Current program pid: ");
        uart_putuint64(p->pid);
        uart_putstr("\n");
        
        if (p->state == READY) {
            p->state = RUNNING;
            write_mepc(p->base_address);
            // clear mpp bit to switch to user mode
            clear_mpp();

            // execute mret
            uart_putstr("\n");
            uart_putstr("=== Start executing program ");
            uart_putuint64(p->pid);
            uart_putstr(" ===\n");
            __asm__ __volatile__("mret");

            // remove the program after finish running
            uart_putstr("Finished Running! Removed the program!\n");
            dequeue();
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