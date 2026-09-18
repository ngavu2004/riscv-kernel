// This file contains trap handling logic
#include "csr.h"

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
    uart_putuint64(user_a0);
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

    if (cause == 8) {
        switch (frame->reg[17]) {
            case 1: // Print syscall
                uart_putstr("Print syscall called.\n");
                uart_putuint64(frame->reg[10]);
                uart_putstr("\n");
                break;
            case 10: // Exit.
                uart_putstr("Process exited.\n");
                execute_processes(); // continue execute other processes
                return;
        }
        
    }
    
    uart_putstr("Trap handler called\n");
    write_mepc(read_mepc() + 4);

    return;
}