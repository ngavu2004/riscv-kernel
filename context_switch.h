#ifndef CONTEXT_SWITCH_H
#define CONTEXT_SWITCH_H

#include <stdint.h>
#include "thread.h"

__attribute__((noreturn))
void restore_context_asm(const thread_context_t *context);

void save_context(uint64_t tid, thread_context_t *frame);
void restore_context(uint64_t tid);

#endif
