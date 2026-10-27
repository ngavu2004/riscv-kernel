#include "csr.h"
#include "timer.h"
#include "hart.h"

static uint64_t next_deadline[MAX_HART_NUM];

uint64_t timer_now(void)
{
    uint64_t time;
    __asm__ volatile("csrr %0, time" : "=r"(time));
    return time;
}

void timer_init(void)
{
    uint64_t hart_id = current_hart_id;

    next_deadline[hart_id] = timer_now() + TIMER_SLICE;
    write_stimecmp(next_deadline[hart_id]);
    enable_timer_supervisor_interrupt();
}

void timer_rearm(void)
{
    uint64_t hart_id = current_hart_id;
    uint64_t now = timer_now();
    next_deadline[hart_id] += TIMER_SLICE;
    
    if (next_deadline[hart_id] <= now) {
        next_deadline[hart_id] = now + TIMER_SLICE;
    } else {
        next_deadline[hart_id] += TIMER_SLICE;
    }

    write_stimecmp(next_deadline[hart_id]);
}
