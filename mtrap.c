#include "uart.h"
#include "csr.h"

__asm__(".p2align 2"); // Aligns to 2^2 = 4 bytes
void mtrap_handler(void) {
    mcause_value_t cause = read_mcause();

    uart_putstr_direct("Mcause interrupt bit: ");
    uart_puthex_direct(cause.interrupt_bit);
    uart_putstr_direct("\n");

    uart_putstr_direct("Mcause exception code:");
    uart_puthex_direct(cause.exception_code);
    uart_putstr_direct("\n");

    if (cause.interrupt_bit == 0) {
        switch (cause.exception_code) {
            case 0:
                uart_putstr_direct("Instruction address misaligned");
                uart_putstr_direct("\n");
                break;
            case 1:
                uart_putstr_direct("Instruction access fault");
                uart_putstr_direct("\n");
                break;
            case 2:
                uart_putline_direct("Illegal instruction");
                break;
            case 7:
                uart_putline_direct("Store/AMO access fault");
                break;
            case 8:
                uart_putline_direct("Environment call from U-Mode");
                break;
        }
    }

    while (1) {

    }
}