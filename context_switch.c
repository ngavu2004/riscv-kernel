#include "context_switch.h"
#include "uart.h"

extern queue_t process_q;
void save_context(uint64_t pid, process_context_t *frame) {
    // Find the process in the process queue
    uart_putstr("PID: ");
    uart_putuint64(pid);
    uart_putline("");

    for(int i = 0; i < PROCESS_TABLE_CAPACITY; i++) {
        process_t *p = &process_table[i];
        uart_putstr("Loop pid: ");
        uart_putuint64(p->pid);
        uart_putline("");

        if (p->pid == pid) {
            // Initialize the save context
            p->have_saved_context = true;
            p->saved_context = *frame;
            uart_putstr("Saved context of process with pid ");
            uart_putuint64(pid);
            uart_putstr("\n");
            break;
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