
#include <stdint.h>
#include <stddef.h>
#define MONITOR_VA 0x500000ULL

uint64_t checksum(const volatile uint8_t *data, size_t len)
{
    uint64_t hash = 14695981039346656037ULL;

    for (size_t i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= 1099511628211ULL;
    }

    return hash;
}

void print(char *s) {
    register char *buffer_register asm("a0") = s;
    register uint64_t syscall_register asm("a7") = 1;

    asm volatile(
        "ecall"
        : "+r"(buffer_register)
        : "r"(syscall_register)
        : "memory"
    );
}

void monitor_main(void)
{
    volatile uint8_t *target_memory = (volatile uint8_t *)0x500000;

    uint64_t previous = checksum(target_memory, 4096);

    while (1) {
        uint64_t current = checksum(target_memory, 4096);

        if (current != previous) {
            // Report memory changed via syscall or UART IPC.
            previous = current;
            print("The memory has changed.");
        }
        // Yield or sleep before checking again.
    }
}
