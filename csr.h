#ifndef CSR_H
#define CSR_H

#include <stdint.h>
#define MPP_BIT_POS 11
#define SPP_BIT_POS 8
#define SIE_BIT_POS 1
#define STIE_BIT_POS 5
#define STCE_BIT_POS 63
#define USER_BASE_ADDRESS 0x80400000
#define USER_SIZE 4096 // 4 KiB

typedef struct {
    uint8_t interrupt_bit;
    uint64_t exception_code;
} mcause_value_t;

static inline uint64_t read_mhartid(void)
{
    uint64_t hart_id;

    __asm__ volatile("csrr %0, mhartid" : "=r"(hart_id));

    return hart_id;
}

static inline uint64_t read_sp(void)
{
    uint64_t stack_pointer;

    __asm__ volatile("mv %0, sp" : "=r"(stack_pointer));

    return stack_pointer;
}

static inline void write_a0(uint64_t value)
{
    __asm__ volatile("mv a0, %0" : : "r"(value) : "a0");
}

static inline void write_mtvec(uint64_t x) {
    __asm__ volatile("csrw mtvec, %0" : : "r"(x));
}

static inline void write_stvec(uint64_t x) {
    __asm__ volatile("csrw stvec, %0" : : "r"(x));
}

// Write to mepc
// Note about mepc: when an interrupt occur, the interrupterd instruction address is saved to mepc
static inline void write_mepc(uint64_t x) {
    __asm__ volatile("csrw mepc, %0" : : "r" (x));
}

static inline void write_sepc(uint64_t x) {
    __asm__ volatile("csrw sepc, %0" : : "r" (x));
}

static inline void set_stce(uint64_t value) {
    uint64_t mask = 1ULL << STCE_BIT_POS;
    if (value & 1ULL) {
        __asm__ volatile("csrs menvcfg, %0" : : "r"(mask));
    } else {
         __asm__ volatile("csrc menvcfg, %0" : : "r"(mask));
    }
}

static inline void enable_timer_supervisor_interrupt(void) {
    __asm__ volatile("csrs sie, %0" : : "r"(1ULL << STIE_BIT_POS));
}

static inline void enable_supervisor_interrupt(void) {
    __asm__ volatile("csrs sstatus, %0" : : "r"(1ULL << SIE_BIT_POS));
}

static inline void enable_supervisor_time_counter(void) {
    __asm__ volatile("csrs mcounteren, %0" : : "r"(1ULL << 1));
}

static inline void write_stimecmp(uint64_t value) {
    __asm__ volatile("csrw stimecmp, %0" : : "r"(value));
}

static uint64_t align_user_size(uint64_t user_size) {
    uint64_t size = 128;

    while (size < user_size) {
        size *= 2;
    }

    return size;
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
    } else if (entry == 3) {
        __asm__ volatile("csrw pmpaddr3, %0" : : "r"(value));
        __asm__ volatile(
            "csrs pmpcfg0, %0"
            :
            : "r"((uint64_t)0x1f << 24)
        );
    }
}

static inline uint64_t read_mtval() {
    uint64_t value;
    __asm__ volatile("csrr %0, mtval" : "=r"(value));
    return value;
}

static inline uint64_t read_stval(void) {
    uint64_t value;
    __asm__ volatile("csrr %0, stval" : "=r"(value));
    return value;
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

static inline void set_mpp(uint64_t value) {
    // 0: U-mode; 1: S-mode; 3 (b11): M-mode.
    uint64_t mpp = (value & 0x3) << MPP_BIT_POS; // left shift to create a bit mask

    __asm__ volatile(
        "csrr t0, mstatus\n\t"        // Read mstatus into register t0
        "li t1, ~0x1800\n\t"         // Load bitmask for bits 11 and 12 (~0001100000000000)
        "and t0, t0, t1\n\t"         // clear those bits
        "or t0, t0, %0\n\t"
        "csrw mstatus, t0"           // Write the modified value back to mstatus
        : : "r" (mpp) 
        : "t0", "t1"             // Clobbered registers
    );
}

static inline void set_spp(uint64_t value) {
    // 0: U-mode; 1: S-mode; 3 (b11): M-mode.
    uint64_t spp = (value & 0x3) << SPP_BIT_POS; // left shift to create a bit mask

    __asm__ volatile(
        "csrr t0, sstatus\n\t"        // Read sstatus into register t0
        "li t1, ~0x100\n\t"         // Load bitmask for bits 8 (~10000000)
        "and t0, t0, t1\n\t"         // clear those bits
        "or t0, t0, %0\n\t"
        "csrw sstatus, t0"           // Write the modified value back to mstatus
        : : "r" (spp) 
        : "t0", "t1"             // Clobbered registers
    );
}

static inline mcause_value_t read_mcause() {
    mcause_value_t cause;
    uint64_t mcause;
    // csrr [rd], mcause: reads the mcause register into register [rd]
    __asm__ volatile("csrr %0, mcause" : "=r"(mcause));
    cause.interrupt_bit = mcause >> 63;
    cause.exception_code = mcause & 0x7FFFFFFFFFFFFFFF;
    return cause;
}

static inline uint64_t read_scause() {
    uint64_t cause;
    // csrr [rd], mcause: reads the mcause register into register [rd]
    __asm__ volatile("csrr %0, scause" : "=r"(cause));
    return cause;
}

static inline uint64_t read_mepc() {
    uint64_t mepc_value;
    // csrr [rd], mcause: reads the mcause register into register [rd]
    __asm__ volatile("csrr %0, mepc" : "=r"(mepc_value));
    return mepc_value;
}

static inline uint64_t read_sepc() {
    uint64_t sepc_value;
    // csrr [rd], mcause: reads the mcause register into register [rd]
    __asm__ volatile("csrr %0, sepc" : "=r"(sepc_value));
    return sepc_value;
}

static inline void write_mscratch(uint64_t value) {
    __asm__ volatile("csrw mscratch, %0" : : "r"(value));
}

static inline void enable_sum(void)
{
    uint64_t sstatus;
    __asm__ volatile("csrr %0, sstatus" : "=r"(sstatus));
    sstatus |= (1ULL << 18); // sstatus.SUM
    __asm__ volatile("csrw sstatus, %0" :: "r"(sstatus) : "memory");
}

static inline void disable_sum(void)
{
    uint64_t sstatus;
    __asm__ volatile("csrr %0, sstatus" : "=r"(sstatus));
    sstatus &= ~(1ULL << 18);
    __asm__ volatile("csrw sstatus, %0" :: "r"(sstatus) : "memory");
}

#endif