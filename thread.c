#include "thread.h"

thread_t thread_table[MAX_THREAD_NUM];
queue_t thread_q;

thread_t NULL_THREAD = {
    .pid = UINT64_MAX,
    .tid = UINT64_MAX,
    .state = EXIT,
    .context = {0}
};

static void *thread_queue_storage[MAX_THREAD_NUM];

void init_thread_queue(void)
{
    queue_init(&thread_q, thread_queue_storage, MAX_THREAD_NUM);
}

thread_t *find_thread(uint64_t tid)
{
    for (int i = 0; i < MAX_THREAD_NUM; i++) {
        if (thread_table[i].tid == tid) {
            return &thread_table[i];
        }
    }

    return &NULL_THREAD;
}

bool block_thread(uint64_t tid)
{
    thread_t *thread = find_thread(tid);

    if (thread == &NULL_THREAD) {
        return false;
    }

    thread->state = BLOCKED;
    return true;
}

bool unblock_thread(uint64_t tid)
{
    thread_t *thread = find_thread(tid);

    if (thread == &NULL_THREAD || thread->state != BLOCKED) {
        return false;
    }

    thread->state = READY;
    return true;
}

int current_thread_pid(void)
{
    thread_t *thread = queue_peek(&thread_q);

    if (thread == NULL) {
        return -1;
    }

    return (int)thread->pid;
}
