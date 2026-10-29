#include "memory_management.h"
uintptr_t kernel_root_pt;

static void zero_page(void *page)
{
    unsigned char *bytes = page;

    for (size_t i = 0; i < PAGE_SIZE; i++) {
        bytes[i] = 0;
    }
}

void *allocate_page(void)
{
    for (int i = 0; i < BITMAP_SIZE; i++) {
        if (page_bitmap[i] != UINT8_MAX) { // if that byte is not full
            for (int j = 0; j < 8; j++) {
                // find the first bit that is 0
                if ((page_bitmap[i] & (uint8_t)(1u << j)) == 0) {
                    uintptr_t page_address =
                        (uintptr_t)PAGE_ALLOC_START +
                        ((8 * i) + j) * (uintptr_t)PAGE_SIZE;
                    void *page = (void *)page_address;
                    page_bitmap[i] |= (uint8_t)(1u << j);
                    zero_page(page);
                    return page;
                }
            }
        }
    }

    return NULL;
}

void free_page(void * page_start) {
    uint64_t page_index = ((uint64_t) page_start - PAGE_ALLOC_START) / PAGE_SIZE;
    uint64_t byte_index = page_index / 8;
    uint8_t bit_index = page_index % 8;

    page_bitmap[byte_index] ^= 1u << bit_index; // set the bit back to 0 with xor
}

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
            uint64_t curr_index = pt_indexes[pt_level];
            pte_t* entry = &curr_pt->entries[curr_index];

            // if it's leaf, map it to the PA
            if (pt_level == 0) {
                *entry = pa_to_pte(pa, flags);
                break;
            }

            // If not leaf and not valid, allocated new pt
            if (!is_valid(entry)) {
                pagetable_t *next_pt = allocate_page();

                if (next_pt == NULL) {
                    uart_putstr_direct("Out of page-table memory\n");
                    while (1) {
                    }
                }

                *entry = pa_to_pte((uintptr_t)next_pt, PTE_V);
            }

            curr_pt = (pagetable_t *)pte_to_address(*entry);
        }

        // move to next page
        va += PAGE_SIZE;
        pa += PAGE_SIZE;
    }
}

void map_kernel_address(pagetable_t* pt) {
    // Map boot section
    map_page(
        pt,
        (uintptr_t)__boot_text_start,
        (uintptr_t)__boot_text_start,
        (int)(((uintptr_t)__boot_text_end -
               (uintptr_t)__boot_text_start) / PAGE_SIZE),
        PTE_V | PTE_R | PTE_X
    );

    // Map uart
    map_page(
        pt,
        0x10000000,
        0x10000000,
        0x100,
        PTE_V | PTE_R | PTE_W
    );

    // Map kernel text
    map_page(pt, (uintptr_t)__text_start, (uintptr_t)__text_start, (int) (__text_end - __text_start) / PAGE_SIZE, PTE_V | PTE_R | PTE_X);

    // Map kernel rodata and data
    map_page(pt, (uintptr_t)__ro_data_start, (uintptr_t)__ro_data_start, (int) (__ro_data_end - __ro_data_start) / PAGE_SIZE, PTE_V | PTE_R );
    map_page(pt, (uintptr_t)__data_start, (uintptr_t)__data_start, (int) (__data_end - __data_start) / PAGE_SIZE, PTE_V | PTE_R | PTE_W);

    // Map bss section
    map_page(pt, (uintptr_t)__bss_start, (uintptr_t)__bss_start, (int) (__bss_end - __bss_start) / PAGE_SIZE, PTE_V | PTE_R | PTE_W);

    // Map page section
    map_page(
        pt,
        PAGE_ALLOC_START,
        PAGE_ALLOC_START,
        (PAGE_ALLOC_END - PAGE_ALLOC_START) / PAGE_SIZE,
        PTE_V | PTE_R | PTE_W
    );
}

void map_user_uart(pagetable_t *pt)
{
    map_page(
        pt,
        UART0,
        UART0,
        1,
        PTE_V | PTE_R | PTE_W | PTE_U
    );

}

uintptr_t create_kernel_pagetable(void) {
    pagetable_t *root_page_table = allocate_page();

    map_kernel_address(root_page_table);

    return (uintptr_t)root_page_table;
}