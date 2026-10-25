#include "hart.h"

hart_t hart_table[MAX_HART_NUM];
static void *hart_thread_queue_storage[MAX_HART_NUM][MAX_THREAD_NUM];

void init_hart_table(void)
{
    for (uint64_t hart_id = 0; hart_id < MAX_HART_NUM; hart_id++) {
        hart_t *hart = &hart_table[hart_id];

        hart->hart_id = hart_id;
        hart->stack_pointer = 0;
        hart->current_thread = &NULL_THREAD;
        queue_init(
            &hart->thread_queue,
            hart_thread_queue_storage[hart_id],
            MAX_THREAD_NUM
        );
    }
}

hart_t *find_hart(uint64_t hart_id)
{
    if (hart_id >= MAX_HART_NUM) {
        return 0;
    }

    return &hart_table[hart_id];
}
