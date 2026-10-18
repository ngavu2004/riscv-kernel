#include "context_switch.h"

extern process_queue_t process_q;
void save_context(uint64_t pid, process_context_t *frame) {
    // Find the process in the process queue
    for(int i = 0; i< process_q.back; i++) {
        process_t *p = &process_q.queue[i];
        if (p->pid == pid) {
            // Initialize the save context

            p->have_saved_context = true;
            p->saved_context = *frame;
        }
    }
}

void restore_context(uint64_t pid) {
    process_t *process = find_process(pid);

    if (process == &NULL_PROCESS) {
        return;
    }

    restore_context_asm(&process->saved_context);
}