#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    void **items;
    size_t capacity;
    size_t front;
    size_t count;
} queue_t;

void queue_init(queue_t *queue, void **storage, size_t capacity);
bool queue_is_empty(const queue_t *queue);
bool queue_is_full(const queue_t *queue);
bool queue_enqueue(queue_t *queue, void *item);
void *queue_dequeue(queue_t *queue);
void *queue_peek(const queue_t *queue);

#endif