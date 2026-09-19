#include "message.h"
#include <stddef.h>

#define MAX_PROCESSES 4

message_t message_pool[MAX_PROCESSES][MESSAGE_COUNT];
message_queue_t mq[MAX_PROCESSES];

void initialize_message_queue(int p_num) {
    if (p_num > MAX_PROCESSES) {
        p_num = MAX_PROCESSES;
    }

    for (int i = 0; i < p_num; i++) {
        mq[i].message_count = 0;
        // I might have to implement memcpy soon T_T
        for (int j = 0; j < MESSAGE_COUNT; j++) {
            message_pool[i][j].source_pid = -1;
            message_pool[i][j].dest_pid = -1;

            for (size_t k = 0; k < MESSAGE_SIZE; k++) {
                message_pool[i][j].message[k] = '\0';
            }

            mq[i].queue[j] = &message_pool[i][j];
        }
    }
};

bool message_queue_is_full(message_queue_t q) {
    return (q.message_count == MESSAGE_COUNT);
}

bool send(char* text, int source_pid, int dest_pid) {
    if (dest_pid < 0 || dest_pid >= MAX_PROCESSES || text == NULL) {
        return false;
    }

    message_queue_t *destination = &mq[dest_pid];

    if (message_queue_is_full(* destination)) {
        return false;
    };

    // Initialize the new message
    int position = destination->message_count;
    message_t *new_message = destination->queue[position];

    new_message->source_pid = source_pid;
    new_message->dest_pid = dest_pid;

    size_t i = 0;
    for (; i < MESSAGE_SIZE - 1 && text[i] != '\0'; i++) {
        // Note: I really have to implement memcpy
        new_message->message[i] = text[i];
    }
    new_message->message[i] = '\0';

    destination->message_count++;

    return true;
}
