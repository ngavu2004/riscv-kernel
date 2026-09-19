#include "process.h"
#include "csr.h"
#include "uart.h"

process_t process_q[USER_PROCESS_SLOT_NUM];
int queue_pointer = -1;

int enqueue(process_t p) {
    // If the queue is already full
    if (process_queue_is_full()) {
        return -1;
    }

    queue_pointer++;
    process_q[queue_pointer] = p;
    return queue_pointer;
}

process_t* dequeue() {
    if (!is_empty()) {
        process_t* p = &process_q[queue_pointer];
        queue_pointer--;

        return p;
    } else {
        return &NULL_PROCESS;
    };
}

void execute_processes() {
    while (!is_empty()) {
        uart_putstr("The process queue is not empty.\n");
        process_t* p = dequeue();
        uart_putstr("Curr entry point: ");
        uart_puthex(p->base_address);
        uart_putstr("\n");
        
        if (p->state == READY) {
            write_mepc(p->base_address);
            // clear mpp bit to switch to user mode
            clear_mpp();

            // execute mret
            uart_putstr("Start executing the program\n");
            __asm__ __volatile__("mret");
        }
    }
}

bool is_empty() {
    return (queue_pointer < 0);
}

bool process_queue_is_full() {
    return (queue_pointer == USER_PROCESS_SLOT_NUM - 1);
}

process_t NULL_PROCESS = {0, EXIT};