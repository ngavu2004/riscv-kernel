#ifndef UART_USER_H
#define UART_USER_H

#include <stdint.h>

#define UART0 0x10000000UL
#define UART0_SIZE 0x100

void uart_putchar(char c);
void uart_putstr(const char *s);
void uart_putuint64(uint64_t num);
void uart_puthex(uint64_t value);

#endif