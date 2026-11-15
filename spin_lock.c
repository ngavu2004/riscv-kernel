#include "spin_lock.h"
#include "uart.h"

void spin_lock_acquire(spin_lock_t *lock)
{
    uart_put_debug("Start spin lock");
    if (MAX_HART_NUM > 1) {
        while (__atomic_exchange_n(&lock->value, 1, __ATOMIC_ACQUIRE) != 0) {
            __asm__ volatile("nop");
        }
    }
    
}

void spin_lock_release(spin_lock_t *lock)
{
    if (MAX_HART_NUM > 1) {
        __atomic_store_n(&lock->value, 0, __ATOMIC_RELEASE);
    }
}
