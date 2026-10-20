#include "queue.h"

void queue_init(queue_t *queue, void **storage, size_t capacity)
{
    queue->items = storage;
    queue->capacity = capacity;
    queue->front = 0;
    queue->count = 0;
}

bool queue_is_empty(const queue_t *queue)
{
    return queue->count == 0;
}

bool queue_is_full(const queue_t *queue)
{
    return queue->count == queue->capacity;
}

bool queue_enqueue(queue_t *queue, void *item)
{
    if (queue_is_full(queue)) {
        return false;
    }

    size_t back = (queue->front + queue->count) % queue->capacity;
    queue->items[back] = item;
    queue->count++;

    return true;
}

void *queue_dequeue(queue_t *queue)
{
    if (queue_is_empty(queue)) {
        return NULL;
    }

    void *item = queue->items[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->count--;

    return item;
}

void *queue_peek(const queue_t *queue)
{
    if (queue_is_empty(queue)) {
        return NULL;
    }

    return queue->items[queue->front];
}