#include "uart.h"
#include "csr.h"

__asm__(".p2align 2"); // Aligns to 2^2 = 4 bytes
void mtrap_handler(void) {
    mcause_value_t cause = read_mcause();

    uart_putstr("Mcause interrupt bit: ");
    uart_puthex(cause.interrupt_bit);
    uart_putstr("\n");

    uart_putstr("Mcause exception code:");
    uart_puthex(cause.exception_code);
    uart_putstr("\n");

    if (cause.interrupt_bit == 0) {
        switch (cause.exception_code) {
            case 0:
                uart_putstr("Instruction address misaligned");
                uart_putstr("\n");
                break;
            case 1:
                uart_putstr("Instruction access fault");
                uart_putstr("\n");
                break;
            case 2:
                uart_putline("Illegal instruction");
                break;
            case 7:
                uart_putline("Store/AMO access fault");
                break;
            case 8:
                uart_putline("Environment call from U-Mode");
                break;
        }
    }

    while (1) {

    }
}