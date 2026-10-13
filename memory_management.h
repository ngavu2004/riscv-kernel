#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define PTE_V  (1ULL << 0)
#define PTE_R  (1ULL << 1)
#define PTE_W  (1ULL << 2)
#define PTE_X  (1ULL << 3)
#define PTE_U  (1ULL << 4)

#define PTE_PPN_SHIFT 10


#define PAGE_SIZE 4096
#define ENTRY_NUM 512
#define PAGE_START_ADDRESS 0x81000000
#define KERNEL_START 0x80000000
#define KERNEL_END 0x80003000

typedef struct {
    uint8_t data[PAGE_SIZE]
} Page;

typedef uint64_t pte_t; // cant use bit field bc it doesnt guarantee the position of the bit matches the hardware requirements

typedef struct {
    pte_t entries[ENTRY_NUM];
} pagetable_t;


static uintptr_t next_free_page = PAGE_START_ADDRESS;

static void zero_page(void *page)
{
    unsigned char *bytes = page;

    for (size_t i = 0; i < PAGE_SIZE; i++) {
        bytes[i] = 0;
    }
}

static void *page_allocate(void)
{
    void *page = (void *)next_free_page;
    next_free_page += PAGE_SIZE;

    zero_page(page);
    return page;
}


static inline pte_t make_pte(uintptr_t physical_address, uint64_t flags)
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

pagetable_t create_kernel_pagetable(void);
pagetable_t create_user_pagetable(void);

void map_page(
    pagetable_t* pt,
    uint64_t va,
    uint64_t pa,
    uint64_t flags
);

void *memcpy(void *destination, const void *source, size_t size);
