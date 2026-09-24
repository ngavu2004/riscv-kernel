#include <stdint.h>
#define MPP_BIT_POS 11
#define USER_BASE_ADDRESS 0x80100000
#define USER_SIZE 4096 // 4 KiB

typedef struct {

} saved_registers;

static inline void write_mtvec(uint64_t x) {
    __asm__ volatile("csrw mtvec, %0" : : "r"(x));
}

// Write to mepc
// Note about mepc: when an interrupt occur, the interrupterd instruction address is saved to mepc
static inline void write_mepc(uint64_t x) {
    __asm__ volatile("csrw mepc, %0" : : "r" (x));
}

static uint64_t align_user_size(uint64_t user_size) {
    uint64_t curr = 128;

    for (int i = 7; i < 64; i++) {
        if (curr < user_size && curr*2 > user_size) {
            return curr*2;
        }
        curr *= 2;
    }
    return 4096;
}

static uint64_t align_base_address(uint64_t base_addr) {
    if (base_addr % 4 != 0) {
        base_addr = (base_addr >> 2) << 2;
    }

    return base_addr;
}

static inline void write_pmpaddr(unsigned int entry, uint64_t value) {
    if (entry == 0) {
        __asm__ volatile("csrw pmpaddr0, %0" : : "r"(value));
        __asm__ volatile("csrs pmpcfg0, %0" : : "r"(0x1f));
    } else if (entry == 1) {
        __asm__ volatile("csrw pmpaddr1, %0" : : "r"(value));
        __asm__ volatile("csrs pmpcfg0, %0" : : "r"(0x1f << 8));
    } else if (entry == 2) {
        __asm__ volatile("csrw pmpaddr2, %0" : : "r"(value));
        __asm__ volatile("csrs pmpcfg0, %0" : : "r"(0x1f << (8*2)));
    }
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

static inline void write_mscratch(uint64_t value) {
    __asm__ volatile("csrw mscratch, %0" : : "r"(value));
}