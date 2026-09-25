#include <stdint.h>
#include "uart_user.h"

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

    while (i > 0) {
        uart_putchar(buffer[i--]);
    }

}

void uart_puthex(uint64_t value) {
    const char digits[] = "0123456789abcdef";
    int shift;

    uart_putstr("0x");

    for (shift = 60; shift >= 0; shift -= 4) {
        uart_putchar(digits[(value >> shift) & 0xF]);
    }
}

static void syscall_receive(char *buffer) {
    register char *buffer_register asm("a0") = buffer;
    register uint64_t syscall_register asm("a7") = 3;

    asm volatile(
        "ecall"
        : "+r"(buffer_register)
        : "r"(syscall_register)
        : "memory"
    );
}

static void syscall_exit(void) {
    register uint64_t syscall_register asm("a7") = 10;

    asm volatile(
        "ecall"
        :
        : "r"(syscall_register)
        : "memory"
    );
}

void user_main() {
    char buffer[128];

    syscall_receive(buffer);
    uart_putstr("User UART received: ");
    uart_putstr(buffer);
    uart_putstr("\n");

    syscall_exit();

    while (1) {
    }
}