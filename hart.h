#ifndef HART_H
#define HART_H

#include <stdint.h>
#include <stdbool.h>
#include "my_lib/queue.h"
#include "thread.h"

#define MAX_HART_NUM 2
#define MAIN_HARTID 0

typedef struct {
    uint64_t hart_id;
    uintptr_t stack_pointer;
    thread_t *current_thread;
    queue_t thread_queue;
} hart_t;

extern hart_t hart_table[MAX_HART_NUM];
extern volatile bool hart_release_table[MAX_HART_NUM];
extern uint64_t current_hart_id;

void init_hart_table(void);
hart_t *find_hart(uint64_t hart_id);
void wait_for_hart(uint64_t hart_id);
void release_hart(uint64_t hart_id);
queue_t *current_hart_thread_queue(void);

#endif
