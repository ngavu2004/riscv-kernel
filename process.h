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
    uint64_t pid;
    uint64_t base_address;
    process_state state;
} process_t;

typedef struct {
    process_t queue[USER_PROCESS_SLOT_NUM];
    int front;
    int back;
} process_queue_t;

extern process_t NULL_PROCESS;

void execute_processes();

// Process queue methods
int enqueue(process_t p);
process_t* front();
process_t* dequeue();
bool process_queue_is_empty();
bool process_queue_is_full();
int process_queue_curr_pid(); // return current running process pid