#include <stdint.h>
#include <stdbool.h>

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
    uint64_t base_address;
    process_states state;
} process_t;

process_t NULL_PROCESS = {0, EXIT};

int enqueue(process_t p);
process_t* dequeue();
bool is_empty();
bool is_full();