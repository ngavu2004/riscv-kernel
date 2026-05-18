#include <stdint.h>

// QEMU emulator defines UART at 0x10000000L
#define UART0 0x10000000L

void uart_putchar(char c) {
    *(volatile unsigned char*)(UART0) = c;
}

void uart_putstr(const char* s) {
    while (*s) {
        uart_putchar(*s++);
    }
}