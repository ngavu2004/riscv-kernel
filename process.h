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
} process_state;

// Define process struct
typedef struct {
    uint64_t base_address;
    process_state state;
} process_t;

extern process_t NULL_PROCESS;

void execute_processes();
int enqueue(process_t p);
process_t* dequeue();
bool is_empty();
bool process_queue_is_full();