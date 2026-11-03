#include <stdint.h>

#define SHARED_PAGE_MONITOR 0x500000ULL

static void delay(void)
{
    for (volatile uint64_t i = 0; i < 1000000; i++) {
    }
}

void observed_program_main(void)
{
    volatile uint8_t *shared_page =
        (volatile uint8_t *)SHARED_PAGE_MONITOR;

    uint8_t value = 0;

    while (1) {
        shared_page[0] = value++;
        delay();
    }
}