#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "my_lib/queue.h"
#include "thread.h"

#define PROCESS_TABLE_CAPACITY 16

typedef enum {
    PROCESS_ACTIVE,
    PROCESS_EXIT
} process_state;

typedef struct {
    uint64_t pid;
    uint64_t base_address;
    process_state state;
    uintptr_t root_pt;
} process_t;

void scheduler(void);

extern process_t NULL_PROCESS;
extern process_t process_table[PROCESS_TABLE_CAPACITY];
extern queue_t process_q;

process_t *find_process(int pid);
void process_queue_init(void);
int process_queue_curr_pid(void);

#endif
