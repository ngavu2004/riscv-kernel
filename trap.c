// This file contains trap handling logic
#include <stdint.h>
#include <stddef.h>
#include "csr.h"
#include "process.h"
#include "message.h"

void uart_putstr(const char*);
void uart_putuint64(uint64_t num);
void uart_puthex(uint64_t num);
void execute_processes();

// The basic things that a trap handler has to do:
// 1. inspect the cause
// 2. detect syscall
// 3. call UART
// 4. increase mepc

typedef struct{
    uint64_t reg[32];
} reg_frame;

void trap_handler(reg_frame *frame) {
    uint64_t user_a0 = frame->reg[10]; // a0 is x10
    uint64_t user_sp = frame->reg[2];  // sp is x2
    uint64_t user_ra = frame->reg[1];  // ra is x1
    
    uart_putstr("User a0: ");
    uart_puthex(user_a0);
    uart_putstr("\n");

    uint64_t cause = read_mcause();

    uart_putstr("Mcause value:");
    uart_puthex(cause);
    uart_putstr("\n");

    if (cause == 1) {
        uart_putstr("Instruction access fault\n");
        return;
    }

    if (cause == 2) {
        uart_putstr("Illegal instruction");
        return;
    }

    if (cause == 7) {
        uart_putstr("Store access fault at: ");
        uart_puthex(read_mtval());
        uart_putstr("\n");

        while (1) {
            __asm__ volatile("wfi");
        }
    }

    if (cause == 8) {
        switch (frame->reg[17]) {
            case 1: // Print syscall
                uart_putstr("Print syscall called.\n");
                uart_putuint64(frame->reg[10]);
                uart_putstr("\n");
                break;
            case 2: // Send
                const char *message = (const char *)frame->reg[10];
                uart_putstr("Sent message: ");
                uart_putstr(message);
                uart_putstr("\n");

                send((char *)frame->reg[10], process_queue_curr_pid(), (int)frame->reg[11]);
                break;
            case 3: // Receive message
                uart_putstr("Start receiving message: ");
                char *user_buffer = (char *)(uintptr_t)user_a0;

                if (user_buffer == NULL) {
                    uart_putstr("Invalid receive buffer\n");
                    break;
                }

                int pid = process_queue_curr_pid();
                receive(pid, user_buffer);
                break;
            case 10: // Exit
                uart_putstr("Process exited.\n");

                // Remove the process that just exited.
                dequeue();

                if (process_queue_is_empty()) {
                    uart_putstr("No more processes.\n");
                    while (1) {
                        __asm__ volatile("wfi");
                    }
                }

                execute_processes();
                return; // Important: skip the common mepc += 4 logic
        }
    }
    
    uart_putstr("Trap handler called\n");
    write_mepc(read_mepc() + 4);
    return;
}