#include <stdint.h>
#define MPP_BIT_POS 11

static inline void write_mtvec(uint64_t x) {
    asm volatile("csrw mtvec, %0" : : "r"(x));
}

static inline void write_mepc(uint64_t x) {
    asm volatile("csrw mepc, %0" : : "r" (x));
}

static inline void clear_mpp() {
    asm volatile(
        "csrr t0, mstatus\n\t"        // Read mstatus into register t0
        "li t1, ~0x1800\n\t"         // Load bitmask for bits 11 and 12 (~0001100000000000)
        "and t0, t0, t1\n\t"         // Perform bitwise AND to clear those bits
        "csrw mstatus, t0"           // Write the modified value back to mstatus
        : : : "t0", "t1"             // Clobbered registers
    );
}

static inline uint64_t read_mcause() {
    uint64_t cause;
    // csrr [rd], mcause: reads the mcause register into register [rd]
    asm volatile("csrr %0, mcause" : "=r"(cause));
    return cause;
}

static inline uint64_t read_mepc() {
    uint64_t mepc_value;
    // csrr [rd], mcause: reads the mcause register into register [rd]
    asm volatile("csrr %0, mcause" : "=r"(mepc_value));
    return mepc_value;
}