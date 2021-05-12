#ifndef _RISCV64_MEMIO_H
#define _RISCV64_MEMIO_H

int hal_memio_remap(unsigned long paddr, unsigned long vaddr, size_t size);
int hal_memio_unmap(unsigned long addr, size_t size);

#endif   /* _RISCV64_MEMIO_H */
