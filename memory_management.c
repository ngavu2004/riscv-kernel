#include "memory_management.h"

void *memcpy(void *destination, const void *source, size_t size) {
    unsigned char *destination_bytes = destination;
    const unsigned char *source_bytes = source;

    for (size_t i = 0; i < size; i++) {
        destination_bytes[i] = source_bytes[i];
    }

    return destination;
}

void map_page(
    pagetable_t* pt,
    uint64_t va,
    uint64_t pa,
    int pages,
    uint64_t flags
) {

    for (int i=0; i < pages; i++) {
        uint64_t pt_indexes[3];
        get_indexes(va, (uint64_t *) pt_indexes);
        
        // Page table walk
        pagetable_t* curr_pt = pt;
        for (int pt_level=2; pt_level >= 0; pt_level--) {

            // If the next pt and index is not valid
            if (!is_valid(&curr_pt->entries[pt_indexes[pt_level]])) {
                // If not leaf, allocate new page
                if (pt_level > 0) {
                    curr_pt->entries[pt_indexes[pt_level]] = pa_to_pte((uintptr_t) page_allocate(), PTE_V);
                } else { // else map it to the pa
                    curr_pt->entries[pt_indexes[pt_level]] = pa_to_pte(pa, flags);
                }
            }
        }

        // move to next page
        va += PAGE_SIZE;
        pa += PAGE_SIZE;
    }
}

void map_kernel_address(pagetable_t* pt) {
    // Map kernel text
    map_page(pt, (uintptr_t)__text_start, (uintptr_t)__text_start, (int) (__text_start - __text_end) / PAGE_SIZE, PTE_V | PTE_R | PTE_X);

    // Map kernel rodata and data
    map_page(pt, (uintptr_t)__ro_data_start, (uintptr_t)__ro_data_end, (int) (__ro_data_start - __ro_data_end) / PAGE_SIZE, PTE_V | PTE_R );
    map_page(pt, (uintptr_t)__data_start, (uintptr_t)__data_end, (int) (__data_start - __data_end) / PAGE_SIZE, PTE_V | PTE_R | PTE_W);

    // Map bss section
    map_page(pt, (uintptr_t)__bss_start, (uintptr_t)__bss_end, (int) (__bss_start - __bss_end) / PAGE_SIZE, PTE_V | PTE_R | PTE_W);
}

uintptr_t create_kernel_pagetable(void) {
    pagetable_t *root_page_table = page_allocate();

    map_kernel_address(root_page_table);

    return (uintptr_t)root_page_table;
}