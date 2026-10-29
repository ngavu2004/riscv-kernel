#ifndef SPIN_LOCK_H
#define SPIN_LOCK_H

#include <stdint.h>

typedef struct {
    uint32_t value;
} spin_lock_t;

void spin_lock_acquire(spin_lock_t *lock);
void spin_lock_release(spin_lock_t *lock);

#endif
