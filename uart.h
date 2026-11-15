#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

extern uint64_t uart_thread_tid;
extern bool uart_user_ready;

bool uart_defined(void);
void uart_set_ready(void);
void uart_putstr_direct(const char *s);
void uart_putline_direct(const char *s);
void uart_putuint64_direct(uint64_t num);
void uart_puthex_direct(uint64_t value);

void uart_putstr(const char *s);
void uart_put_debug(const char *s);
void uart_putline(const char *s);
void uart_putuint64(uint64_t num);
void uart_puthex(uint64_t value);

#endif