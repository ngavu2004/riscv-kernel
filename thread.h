#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>
#include <stdbool.h>

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


#endif