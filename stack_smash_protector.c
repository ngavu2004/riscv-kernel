#include <stdint.h>

#define STACK_CHK_GUARD 0xdeadbeef

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;
void uart_putstr(const char*);

__attribute__((noreturn))
void __stack_chk_fail(void) {
    uart_putstr("Stack smashing detected\n");

    while(1) {
        __asm__ volatile("wfi");
    }
}