#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdbool.h>
#include "my_lib/queue.h"

#define PROCESS_TABLE_CAPACITY 16


void restore_context(uint64_t pid);
// Define process states
// TODO: Change to ACTIVE and EXIT after completely implemented thread
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
// TODO: update when finish the thread struct 
typedef struct {
    uint64_t pid;
    uint64_t base_address;
    process_state state;
    bool have_saved_context;
    process_context_t saved_context;
    uintptr_t root_pt;
} process_t;


void scheduler();
// Define process queue and NULL process
extern process_t NULL_PROCESS;
extern process_t process_table[PROCESS_TABLE_CAPACITY];
extern queue_t process_q;


// process context switch methods
bool block_process(int pid);
bool unblock_process(int pid);
process_t* find_process(int pid);

// Process queue methods
void process_queue_init(void);
int process_queue_curr_pid(); // return current running process pid

#endif