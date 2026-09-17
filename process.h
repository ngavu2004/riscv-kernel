#include <stdio.h>
#include <stdint.h>

#define USER_PROCESS_BASE 0x80100000
#define USER_PROCESS_SLOT_SIZE 0x1000
#define USER_PROCESS_SLOT_NUM 4

// Define process states
typedef enum {
    PENDING,
    READY,
    RUNNING,
    EXIT
} process_states;

// Define process struct
typedef struct {
    uintptr_t base_address;
    process_states state;
} process;