#include <cstdint>

static inline void write_mtvec(uint64_t x) {
    asm volatile("csrw mtvec, %0" : : "r"(x));
}

static inline void write_mepc(uint64_t x) {
    asm volatile("csrw mepc, %0" : : "r" (x));
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