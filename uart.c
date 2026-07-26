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

void uart_putuint64(uint64_t num) {
    char buffer[21];
    int i = 0;

    if (num == 0) {
        uart_putchar('0');
        return;
    }

    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    while (i >= 0) {
        uart_putchar(buffer[i--]);
    }

}