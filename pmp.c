#include "pmp.h"
#include "csr.h"
#include "uart.h"

__attribute__((section(".text.boot")))
void enable_user_memory(uint64_t base, uint64_t size, int entry) {
    uart_put_debug("enable_user_memory line 7\n");
    uint64_t base_addr = align_base_address(base);
    uint64_t aligned_user_size = align_user_size(size);
    uint64_t pmpaddr =
    (base >> 2) | ((aligned_user_size / 8) - 1);
    write_pmpaddr(entry, pmpaddr);
    uart_put_debug("enable_user_memory end of func\n");
}

__attribute__((section(".text.boot")))
void enable_uart_memory() {
    enable_user_memory(UART0, UART0_SIZE, 3);
}