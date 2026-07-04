#include <stdint.h>
#define MPP_BIT_POS 11

static inline void write_mtvec(uint64_t x) {
    __asm__ volatile("csrw mtvec, %0" : : "r"(x));
}

// Write to mepc
// Note about mepc: when an interrupt occur
static inline void write_mepc(uint64_t x) {
    __asm__ volatile("csrw mepc, %0" : : "r" (x));
}

static inline void enable_user_memory() {
    __asm__ volatile(
        "li t0, -1\n\t"
        "csrw pmpaddr0, t0\n\t"
        "li t0, 0x1f\n\t"
        "csrw pmpcfg0, t0"
        : : : "t0"
    );
}

static inline void clear_mpp() {
    __asm__ volatile(
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
    __asm__ volatile("csrr %0, mcause" : "=r"(cause));
    return cause;
}

static inline uint64_t read_mepc() {
    uint64_t mepc_value;
    // csrr [rd], mcause: reads the mcause register into register [rd]
    __asm__ volatile("csrr %0, mepc" : "=r"(mepc_value));
    return mepc_value;
}