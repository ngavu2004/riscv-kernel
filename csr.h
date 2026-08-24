#include <stdint.h>
#define MPP_BIT_POS 11
#define USER_BASE_ADDRESS 0x80100000
#define USER_SIZE 4096 // 4 KiB

static inline void write_mtvec(uint64_t x) {
    __asm__ volatile("csrw mtvec, %0" : : "r"(x));
}

// Write to mepc
// Note about mepc: when an interrupt occur, the interrupterd instruction address is saved to mepc
static inline void write_mepc(uint64_t x) {
    __asm__ volatile("csrw mepc, %0" : : "r" (x));
}

static uint64_t align_user_sign(uint64_t user_size) {
    uint64_t curr = 128;

    for (int i = 7; i < 64; i++) {
        if (curr < user_size && curr*2 > user_size) {
            return curr*2;
        }
        curr *= 2;
    }
    return 4096;
}

static inline void enable_user_memory(uint64_t user_base_address, uint64_t user_size) {
    // Note RISC-V pmpaddr0 and pmpcf0 config the entry Physical Memory Protection
    // pmpcf0 decides the mode. The mode will decide the role of pmpaddr0
    // In our case, pmpcf0 = 0x1f -> 0 0 0 1 1 1 1 1
    // We have R: 1, W: 1, X: 1, and A = 11
    // Since A = 11, the mode is NATO
    // In this mode, the pmpaddr encodes both the base address and user size
    // int pmpaddr = (USER_BASE_ADDRESS >> 2) | ((USER_SIZE / 8) - 1)
    // why (USER_BASE_ADDRESS >> 2) you ask?
    // Because in NATA mode, the pmpaddr is 4-byte aligned, which will always have 00 at the end
    // So we always know the end is gonna be 00, then why wasting on the 00, shift them right by 2
    // and later on we can restore them later
    uint64_t aligned_user_size = align_user_sign(user_size);
    uint64_t pmpaddr = (user_base_address >> 2) | ((aligned_user_size / 8) - 1);
    __asm__ volatile(
        "mv t0, %0\n\t"
        "csrw pmpaddr0, t0\n\t"
        "li t0, 0x1f\n\t"
        "csrw pmpcfg0, t0"
        : :"r"(pmpaddr) : "t0"
    );
}

// 
// mpp: is 2 bits in mstatus that holds information about previous priviledge mode
// This set mpp to 00, which means previous priviledge mode is U-mode, not S-mode
// currently 
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