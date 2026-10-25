#include "csr.h"
#include "timer.h"

static uint64_t next_deadline;

uint64_t timer_now(void)
{
    uint64_t time;
    __asm__ volatile("csrr %0, time" : "=r"(time));
    return time;
}

void timer_init(void)
{
    next_deadline = timer_now() + TIMER_SLICE;
    write_stimecmp(next_deadline);
    enable_timer_supervisor_interrupt();
    enable_supervisor_interrupt();
}

void timer_rearm(void)
{
    next_deadline += TIMER_SLICE;
    write_stimecmp(next_deadline);
}
