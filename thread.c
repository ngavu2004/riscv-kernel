#include "thread.h"
#include "memory_management.h"
#include "process.h"
#include "hart.h"

#define USER_STACK_TOP 0x40000000ULL
#define SSTATUS_SIE  (1ULL << 1)
#define SSTATUS_SPIE   (1ULL << 5)
#define SSTATUS_SPP  (1ULL << 8)

thread_t thread_table[MAX_THREAD_NUM];

thread_t NULL_THREAD = {
    .pid = UINT64_MAX,
    .tid = UINT64_MAX,
    .state = EXIT,
    .context = {0}
};

bool initialize_main_thread(process_t *process, thread_t *thread)
{
    void *stack_page = allocate_page();

    if (stack_page == NULL) {
        return false;
    }

    map_page(
        (pagetable_t *)process->root_pt,
        USER_STACK_TOP - PAGE_SIZE,
        (uintptr_t)stack_page,
        1,
        PTE_V | PTE_R | PTE_W | PTE_U
    );

    thread->pid = process->pid;
    thread->state = READY;
    thread->context.sepc = process->base_address;
    thread->context.reg[2] = USER_STACK_TOP & ~0xFULL;
    thread->context.sstatus = SSTATUS_SPIE;
    thread->timer_preemptible = true;

    return true;
}

thread_t *find_thread(uint64_t tid)
{
    for (int i = 0; i < MAX_THREAD_NUM; i++) {
        if (thread_table[i].tid == tid) {
            return &thread_table[i];
        }
    }

    return &NULL_THREAD;
}

bool block_thread(uint64_t tid)
{
    thread_t *thread = find_thread(tid);

    if (thread == &NULL_THREAD) {
        return false;
    }

    thread->state = BLOCKED;
    return true;
}

bool unblock_thread(uint64_t tid)
{
    thread_t *thread = find_thread(tid);

    if (thread == &NULL_THREAD || thread->state != BLOCKED) {
        return false;
    }

    thread->state = READY;
    return true;
}

int current_thread_pid(void)
{
    queue_t *thread_queue = current_hart_thread_queue();
    thread_t *thread = thread_queue == 0 ? 0 : queue_peek(thread_queue);

    if (thread == NULL) {
        return -1;
    }

    return (int)thread->pid;
}
