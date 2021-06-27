#ifndef _RISCV64_CPU_H
#define _RISCV64_CPU_H

#include <types.h>

#define CPU_NR_MAX  1

void cpu_do_nothing();
void cpu_do_udelay(int usec);

#define cpu_idle        cpu_do_nothing

cpuid_t cpu_get_my_id();

void cpu_get_attached_list(cpuid_t *cpu_list, unsigned int *count);
void cpu_init();

static inline void cpu_do_pause(void)
{
}

#define cpu_pause       cpu_do_pause
#define udelay          cpu_do_udelay

static inline void hartid_init(unsigned long hartid) {
  asm volatile("mv tp, %0" : : "r" (hartid & 0x1));
}

#endif  /* _RISCV64_CPU_H */