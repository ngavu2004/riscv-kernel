#include <stdint.h>
#include "message.h"
#include "uart.h"

uint64_t uart_thread_tid;
static bool uart_user_ready;

static void uart_putchar_direct(char c) {
    *(volatile unsigned char*)(UART0) = c;
}

void uart_set_ready(void)
{
    uart_user_ready = true;
}

void uart_putstr_direct(const char *s)
{
    while (*s) {
        uart_putchar_direct(*s++);
    }
}

void uart_putline_direct(const char *s)
{
    uart_putstr_direct(s);
    uart_putchar_direct('\n');
}

void uart_putuint64_direct(uint64_t num)
{
    char buffer[21];
    int i = 0;

    if (num == 0) {
        uart_putchar_direct('0');
        return;
    }

    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    while (i > 0) {
        uart_putchar_direct(buffer[--i]);
    }
}

void uart_puthex_direct(uint64_t value)
{
    const char digits[] = "0123456789abcdef";
    int shift;

    uart_putchar_direct('0');
    uart_putchar_direct('x');

    for (shift = 60; shift >= 0; shift -= 4) {
        uart_putchar_direct(digits[(value >> shift) & 0xF]);
    }
}

void uart_putstr(const char *s)
{
    if (!uart_user_ready) {
        uart_putstr_direct(s);
        return;
    }

    send((char *)s, -1, (int)uart_thread_tid);
}

void uart_putline(const char *s)
{
    if (!uart_user_ready) {
        uart_putline_direct(s);
        return;
    }

    send((char *)s, -1, (int)uart_thread_tid);
    send((char *)"\n", -1, (int)uart_thread_tid);
}

void uart_putuint64(uint64_t num)
{
    char buffer[21];
    char output[21];
    int i = 0;
    int j = 0;

    if (num == 0) {
        uart_putstr("0");
        return;
    }

    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    while (i > 0) {
        output[j++] = buffer[--i];
    }
    output[j] = '\0';
    uart_putstr(output);
}

void uart_puthex(uint64_t value)
{
    if (!uart_user_ready) {
        uart_puthex_direct(value);
        return;
    }

    char buffer[19];
    const char digits[] = "0123456789abcdef";
    int shift;

    buffer[0] = '0';
    buffer[1] = 'x';
    for (shift = 60; shift >= 0; shift -= 4) {
        buffer[2 + (60 - shift) / 4] = digits[(value >> shift) & 0xF];
    }
    buffer[18] = '\0';
    uart_putstr(buffer);
}