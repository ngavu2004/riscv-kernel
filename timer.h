#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define TIMER_SLICE 100000ULL

void timer_init(void);
void timer_rearm(void);
uint64_t timer_now(void);

#endif
