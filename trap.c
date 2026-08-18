// This file contains trap handling logic
#include "csr.h"

void uart_putstr(const char*);
void uart_putuint64(uint64_t num);

// The basic things that a trap handler has to do:
// 1. inspect the cause
// 2. detect syscall
// 3. call UART
// 4. increase mepc
void trap_handler() {
    uint64_t cause = read_mcause();

    uart_putstr("Mcause value");
    uart_putuint64(cause);

    if (cause == 2) {
        uart_putstr("Illegal instruction");
    }
    
    uart_putstr("Trap handler called");
    
    
    write_mepc(read_mepc() + 4);

    return;
}