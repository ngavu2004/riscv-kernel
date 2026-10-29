#ifndef UART_H
#define UART_H

#include <stdint.h>
// QEMU emulator defines UART at 0x10000000L
#define UART0 0x10000000L
#define UART0_SIZE 0x100

extern uint64_t uart_thread_tid;

void uart_set_ready(void);
void uart_putstr_direct(const char *s);
void uart_putline_direct(const char *s);
void uart_putuint64_direct(uint64_t num);
void uart_puthex_direct(uint64_t value);

void uart_putstr(const char *s);
void uart_putline(const char *s);
void uart_putuint64(uint64_t num);
void uart_puthex(uint64_t value);

#endif