#include "process.h"
#include <stdbool.h>

process process_q[USER_PROCESS_SLOT_NUM];
int queue_pointer = -1;

int enqueue(process p) {
    // If the queue is already full
    if (is_full()) {
        return -1;
    }

    queue_pointer++;
    process_q[queue_pointer] = p;
    return queue_pointer;
}

int dequeue() {
    if (!is_empty()) {
        queue_pointer--;
    };

    return queue_pointer;
}

bool is_empty() {
    return (queue_pointer < 0);
}

bool is_full() {
    return (queue_pointer == USER_PROCESS_SLOT_NUM - 1);
}