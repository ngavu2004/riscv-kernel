#include "message.h"
#include "process.h"
#include "thread.h"
#include <stddef.h>

#define MAX_THREADS 4

message_t message_pool[MAX_THREADS][MESSAGE_COUNT];
message_queue_t mq[MAX_THREADS];

void initialize_message_queue(int thread_num) {
    if (thread_num > MAX_THREADS) {
        thread_num = MAX_THREADS;
    }

    for (int i = 0; i < thread_num; i++) {
        mq[i].message_count = 0;
        // I might have to implement memcpy soon T_T
        for (int j = 0; j < MESSAGE_COUNT; j++) {
            message_pool[i][j].source_tid = -1;
            message_pool[i][j].dest_tid = -1;

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

bool send(char* text, int source_tid, int dest_tid) {
    if (dest_tid < 0 || dest_tid >= MAX_THREADS || text == NULL) {
        return false;
    }

    message_queue_t *destination = &mq[dest_tid];

    if (message_queue_is_full(* destination)) {
        return false;
    };
    
    // Initialize the new message
    int position = destination->message_count;
    message_t *new_message = destination->queue[position];

    new_message->source_tid = source_tid;
    new_message->dest_tid = dest_tid;

    size_t i = 0;
    for (; i < MESSAGE_SIZE - 1 && text[i] != '\0'; i++) {
        // Note: I really have to implement memcpy
        new_message->message[i] = text[i];
    }
    new_message->message[i] = '\0';
    destination->message_count++;
    // Wake up the dest_tid to handle the message
    unblock_thread((uint64_t)dest_tid);

    return true;
}

void receive(int curr_tid, char *buffer) {
    message_queue_t* q = &mq[curr_tid];
    // uart_putstr("Current number of message: ");
    // uart_putuint64(q->message_count);
    // uart_putline("");
    
    while (q->message_count > 0) {
        int curr_pos = q->message_count - 1;
        message_t* receive_m = q->queue[curr_pos];
        // uart_putstr("Sender message: ");
        // uart_putstr(receive_m->message);
        // uart_putstr(" ; From: process ");
        // uart_putuint64(receive_m->source_tid + 1);
        // uart_putstr("\n");

        // Copy the message to the buffer
        for(int i=0; i< MESSAGE_SIZE; i++) {
            if (receive_m->message[i] == '\0') {
                buffer[i] = '\0';
                break;
            }

            buffer[i] = receive_m->message[i];
        }

        // Make the receiver ready
        unblock_thread((uint64_t)receive_m->source_tid);
        
        // Reset the message
        receive_m->source_tid = -1;
        receive_m->dest_tid = -1;
        for (size_t k = 0; k < MESSAGE_SIZE; k++) {
           receive_m->message[k] = '\0';
        }
        q->message_count--;
    }
};
