#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>
#include <stdbool.h>
#include "my_lib/queue.h"

#define MAX_THREAD_NUM 16

typedef enum {
    PENDING,
    READY,
    RUNNING,
    BLOCKED,
    EXIT
} thread_state_t;

typedef struct {
    uint64_t reg[32];
    uint64_t sepc;
    uint64_t sstatus;
} thread_context_t;

typedef struct {
    uint64_t pid;
    uint64_t tid;
    thread_state_t state;
    thread_context_t context;
} thread_t;

// Define the thread queue and null thread.
extern thread_t NULL_THREAD;
extern thread_t thread_table[MAX_THREAD_NUM];
extern queue_t thread_q;

bool block_thread(uint64_t tid);
bool unblock_thread(uint64_t tid);
thread_t *find_thread(uint64_t tid);


void init_thread_queue(void);
int current_thread_pid(void);

#endif