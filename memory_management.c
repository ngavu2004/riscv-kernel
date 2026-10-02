#include "memory_management.h"

void *memcpy(void *destination, const void *source, size_t size) {
    unsigned char *destination_bytes = destination;
    const unsigned char *source_bytes = source;

    for (size_t i = 0; i < size; i++) {
        destination_bytes[i] = source_bytes[i];
    }

    return destination;
}