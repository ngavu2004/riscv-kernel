#include "pmp.h"
#include "csr.h"
#include "uart.h"

void enable_user_memory(uint64_t base, uint64_t size, int entry) {
    uint64_t base_addr = align_base_address(base);
    uint64_t aligned_user_size = align_user_size(size);
    uint64_t pmpaddr =
    (base >> 2) | ((aligned_user_size / 8) - 1);

    write_pmpaddr(entry, pmpaddr);
}

void enable_uart_memory() {
    enable_user_memory(UART0, UART0_SIZE, 2);
}