#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_putstr(const char *s);
void uart_putuint64(uint64_t num);
void uart_puthex(uint64_t value);

#endif