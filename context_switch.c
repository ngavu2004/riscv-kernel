#include "context_switch.h"
#include "uart.h"
#include "thread.h"

void save_context(uint64_t tid, thread_context_t *frame) {
    thread_t *thread = find_thread(tid);

    if (thread == &NULL_THREAD) {
        return;
    }

    thread->context = *frame;
}

void restore_context(uint64_t tid) {
    thread_t *t = find_thread(tid);

    if (t == &NULL_THREAD) {
        return;
    }

    restore_context_asm(&t->context);
}