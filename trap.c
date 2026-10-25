// This file contains trap handling logic
#include <stdint.h>
#include <stddef.h>
#include "context_switch.h"
#include "csr.h"
#include "process.h"
#include "message.h"
#include "uart.h"
#include "thread.h"
#include "timer.h"

// The basic things that a trap handler has to do:
// 1. inspect the cause
// 2. detect syscall
// 3. call UART
// 4. increase sepc

void trap_handler(thread_context_t *frame) {
    uint64_t user_a0 = frame->reg[10]; // a0 is x10
    uint64_t user_sp = frame->reg[2];  // sp is x2
    uint64_t user_ra = frame->reg[1];  // ra is x1
    
    uart_putstr("User a0: ");
    uart_puthex(user_a0);
    uart_putstr("\n");

    uint64_t cause = read_scause();

    uart_putstr("Scause value:");
    uart_puthex(cause);
    uart_putstr("\n");

    if (cause >> 63) {
        uint64_t code = cause & 0x7FFFFFFFFFFFFFFFULL;
        if (code == 5) {
            uart_putline("Timer interrupt!");

            // Save context of current running thread, set status to READY, and put it at the end of the queue
            thread_t *current = queue_dequeue(&thread_q);

            if (current != NULL) {
                save_context(current->tid, frame);
                current->state = READY;
                queue_enqueue(&thread_q, current);
            }

            timer_rearm();
            scheduler();
            return;
        }
    } else {
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
            // if the interrupt was created by ecall, increment the sepc by 4
            frame->sepc += 4;
            thread_t *current_thread = queue_peek(&thread_q);
            int curr_tid = current_thread == NULL ? -1 : (int)current_thread->tid;
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


                    send((char *)frame->reg[10], curr_tid, (int)frame->reg[11]);

                    save_context((uint64_t)curr_tid, frame);
                    block_thread((uint64_t)curr_tid); // block the thread after send, until the endpoint receive the message
                    scheduler();
                    break;
                case 3: // Receive message
                    uart_putstr("Start receiving message: ");
                    char *user_buffer = (char *)(uintptr_t)user_a0;

                    if (user_buffer == NULL) {
                        uart_putstr("Invalid receive buffer\n");
                        break;
                    }

                    receive(curr_tid, user_buffer);

                    break;
                case 4: // Yield syscall
                    thread_t *current = queue_dequeue(&thread_q);

                    if (current != NULL) {
                        save_context((uint64_t)curr_tid, frame);
                        current->state = READY;
                        queue_enqueue(&thread_q, current);
                    }

                    scheduler();
                    return;

                case 10: // Exit
                    uart_putstr("Thread exited.\n");
                    thread_t *thread = find_thread((uint64_t)curr_tid);
                    thread->state = EXIT;

                    scheduler();
                    return; // Important: skip the common sepc += 4 logic
            }
        }

        if (cause == 15) {
            uart_putstr("Store page fault at: ");
            uart_puthex(read_stval());
            uart_putstr("\n");

            while (1) {
                __asm__ volatile("wfi");
            }
        }
    }

    
    
    uart_putstr("Trap handler called\n");
    write_sepc(frame->sepc);
    return;
}