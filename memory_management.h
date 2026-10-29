#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "uart.h"
#include "spin_lock.h"

#define PTE_V  (1ULL << 0)
#define PTE_R  (1ULL << 1)
#define PTE_W  (1ULL << 2)
#define PTE_X  (1ULL << 3)
#define PTE_U  (1ULL << 4)

#define PTE_PPN_SHIFT 10

#define PAGE_SIZE 4096
#define ENTRY_NUM 512
#define PAGE_ALLOC_START 0x80400000
#define PAGE_ALLOC_END   0x80800000
#define PAGE_COUNT ((PAGE_ALLOC_END - PAGE_ALLOC_START) / PAGE_SIZE)
#define BITMAP_SIZE ((PAGE_COUNT + 7) / 8) // make sure the bit is rounded up

extern uint8_t page_bitmap[]; // 0 = free; 1 = allocated

extern char __boot_text_start[], __boot_text_end[];
extern char __text_start[], __text_end[];
extern char __ro_data_start[], __ro_data_end[];
extern char __data_start[], __data_end[];
extern char __bss_start[], __bss_end[];

extern uintptr_t kernel_root_pt;

typedef struct {
    uint8_t data[PAGE_SIZE];
} Page;

typedef uint64_t pte_t; // cant use bit field bc it doesnt guarantee the position of the bit matches the hardware requirements

typedef struct {
    pte_t entries[ENTRY_NUM];
} pagetable_t;


void *allocate_page(void);
void free_page(void *);
static inline pte_t pa_to_pte(uintptr_t physical_address, uint64_t flags)
{
    return ((physical_address >> 12) << PTE_PPN_SHIFT) | flags;
}


static inline uintptr_t pte_to_address(pte_t pte)
{
    return ((pte >> PTE_PPN_SHIFT) << 12);
}

static inline bool is_valid(pte_t* a) {
    return (*a & PTE_V);
}

static inline void get_indexes(uint64_t va, uint64_t page_table_indexes[3]) {
    page_table_indexes[2] = (va >> 30) & 0x1ff;
    page_table_indexes[1] = (va >> 21) & 0x1ff;
    page_table_indexes[0] = (va >> 12) & 0x1ff;
}

static inline void switch_page_table(uintptr_t root_pt)
{
    uint64_t satp = (8ULL << 60) | (root_pt >> 12);

    __asm__ volatile(
        "csrw satp, %0\n"
        "sfence.vma"
        :
        : "r"(satp)
        : "memory"
    );
}

uintptr_t create_kernel_pagetable(void);
void map_kernel_address(pagetable_t* pt);

void map_page(
    pagetable_t* pt,
    uint64_t va,
    uint64_t pa,
    int pages,
    uint64_t flags
);

void map_user_uart(pagetable_t *pt);

void *memcpy(void *destination, const void *source, size_t size);

uintptr_t lookup_physical_page(pagetable_t* root, uintptr_t va);
