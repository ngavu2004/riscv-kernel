#include "process.h"

process_t process_q[USER_PROCESS_SLOT_NUM];
int queue_pointer = -1;

int enqueue(process_t p) {
    // If the queue is already full
    if (is_full()) {
        return -1;
    }

    queue_pointer++;
    process_q[queue_pointer] = p;
    return queue_pointer;
}

process_t* dequeue() {
    if (!is_empty()) {
        process_t* p = &process_q[queue_pointer];
        queue_pointer--;

        return p;
    } else {
        return &NULL_PROCESS;
    };
}

bool is_empty() {
    return (queue_pointer < 0);
}

bool is_full() {
    return (queue_pointer == USER_PROCESS_SLOT_NUM - 1);
}