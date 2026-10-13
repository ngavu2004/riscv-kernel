#include "memory_management.h"

void *memcpy(void *destination, const void *source, size_t size) {
    unsigned char *destination_bytes = destination;
    const unsigned char *source_bytes = source;

    for (size_t i = 0; i < size; i++) {
        destination_bytes[i] = source_bytes[i];
    }

    return destination;
}

pagetable_t create_kernel_pagetable(void) {
    pagetable_t root_page_table = *(pagetable_t *) page_allocate();

    // The boot and kernel will take every space from KERNEL_START to KERNEL_END
    

    return root_page_table;
}