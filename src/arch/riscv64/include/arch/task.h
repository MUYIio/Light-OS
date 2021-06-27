#ifndef _RISCV64_TASK_H
#define _RISCV64_TASK_H

#include <types.h>
#include <stdint.h>
#include "interrupt.h"

/* TODO: 修改为RISCV的结构 */
typedef struct {
    uint64_t ra;        // PC
    uint64_t sp;        // SP

    // callee-saved
    uint64_t s0;
    uint64_t s1;
    uint64_t s2;
    uint64_t s3;
    uint64_t s4;
    uint64_t s5;
    uint64_t s6;
    uint64_t s7;
    uint64_t s8;
    uint64_t s9;
    uint64_t s10;
    uint64_t s11;
} thread_stack_t;

/* 任务上下文 */
typedef struct {
    uint64_t ra;        // PC
    uint64_t sp;        // SP

    // callee-saved
    uint64_t s0;
    uint64_t s1;
    uint64_t s2;
    uint64_t s3;
    uint64_t s4;
    uint64_t s5;
    uint64_t s6;
    uint64_t s7;
    uint64_t s8;
    uint64_t s9;
    uint64_t s10;
    uint64_t s11;
} task_context_t;

void thread_switch_to_next(void *prev, void *next);
void thread_kstack_dump(thread_stack_t *kstack);

void kernel_switch_to_user(trap_frame_t *frame);
void kernel_frame_init(trap_frame_t *frame);

#define user_set_entry_point(frame, entry) (frame)->epc = (entry)
void user_frame_init(trap_frame_t *frame);
void user_thread_frame_build(trap_frame_t *frame, void *arg, void *func,
    void *thread_entry, void *stack_top);

#endif  /* _RISCV64_TASK_H */