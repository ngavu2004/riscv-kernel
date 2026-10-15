#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdbool.h>

#define USER_PROCESS_BASE 0x80100000
#define USER_PROCESS_SLOT_SIZE 0x1000
#define USER_PROCESS_SLOT_NUM 4


void restore_context(uint64_t pid);
// Define process states
typedef enum {
    PENDING,
    READY,
    RUNNING,
    BLOCKED,
    EXIT
} process_state;

typedef struct {
    uint64_t reg[32];
    uint64_t sepc;
    uint64_t sstatus;
} process_context_t;

// Define process struct
typedef struct {
    uint64_t pid;
    uint64_t base_address;
    process_state state;
    bool have_saved_context;
    process_context_t saved_context;
    uintptr_t root_pt;
} process_t;

typedef struct {
    process_t queue[USER_PROCESS_SLOT_NUM];
    int front;
    int back;
} process_queue_t;

void scheduler();
// Define process queue and NULL process
extern process_t NULL_PROCESS;
extern process_queue_t process_q;

// process context switch methods
bool block_process(int pid);
bool unblock_process(int pid);
process_t* find_process(int pid);

// Process queue methods
int enqueue(process_t p);
process_t* front();
process_t* dequeue();
bool process_queue_is_empty();
bool process_queue_is_full();
int process_queue_curr_pid(); // return current running process pid

#endif