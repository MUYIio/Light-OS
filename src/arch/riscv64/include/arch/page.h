#ifndef _RISCV64_PAGE_H
#define _RISCV64_PAGE_H

#include <xbook/config.h>
#include "mempool.h"
#include "riscv.h"
#include <stdint.h>
#include <stdbool.h>

typedef unsigned long pte_t; /* page table entry */
typedef unsigned long* pgdir_t; // 512 PTEs

/* 内核空间在映射后的虚拟基地址 */
#define KERN_BASE_VIR_ADDR      0X80000000

#define	PAGE_ATTR_PRESENT	  	(1L << 0) // valid
#define	PAGE_ATTR_READ  	    (1L << 1)
#define	PAGE_ATTR_WRITE  	    (1L << 2)
#define	PAGE_ATTR_EXEC  	    (1L << 3)
#define	PAGE_ATTR_USER  	    (1L << 4)   // 1 -> user can access
#define	PAGE_ATTR_SYSTEM  	    (0)

#define KERN_PAGE_ATTR  (PAGE_ATTR_PRESENT | PAGE_ATTR_WRITE | PAGE_ATTR_WRITE | PAGE_ATTR_SYSTEM)

#define PAGE_ATTR_MASK      (PAGE_ATTR_EXEC | PAGE_ATTR_READ | PAGE_ATTR_WRITE | PAGE_ATTR_USER)

#define PAGE_SHIFT  12
#define PAGE_SIZE   4096
#define PAGE_LIMIT  (PAGE_SIZE-1)
#define PAGE_MASK   (~PAGE_LIMIT)  
#define PAGE_ALIGN(value) (((value) + PAGE_LIMIT) & PAGE_MASK)

#define PAGE_ROUNDUP(sz)  (((sz)+PAGE_SIZE-1) & ~(PAGE_SIZE-1))
#define PAGE_ROUNDDOWN(a) (((a)) & ~(PAGE_SIZE-1))

// 2^9 = 512 PTEs
#define PAGE_TABLE_ENTRY_NR 512  

// shift a physical address to the right place for a PTE.
#define PA2PTE(pa) ((((uint64)pa) >> 12) << 10)

#define PTE2PA(pte) (((pte) >> 10) << 12)

#define PTE_FLAGS(pte) ((pte) & 0x3FF)

// extract the three 9-bit page table indices from a virtual address.
#define PXMASK          0x1FF // 9 bits
#define PXSHIFT(level)  (PAGE_SHIFT+(9*(level)))
#define PX(level, va) ((((uint64) (va)) >> PXSHIFT(level)) & PXMASK)


/* 获取页目录表地址 */
#define GET_CUR_PGDIR() ((pgdir_t)SATP_PGTBL(satp_read()))

/* only have MEM_NODE_TYPE_NORMAL */
#define page_alloc_normal(count)            mem_node_alloc_pages(count, MEM_NODE_TYPE_NORMAL)
#define page_alloc_user(count)              mem_node_alloc_pages(count, MEM_NODE_TYPE_NORMAL)
#define page_alloc_dma(count)               mem_node_alloc_pages(count, MEM_NODE_TYPE_NORMAL)
#define page_free(addr)                     mem_node_free_pages(addr)

/* 由于内核的物理地址和虚拟地址是一一映射的，所以物理地址即虚拟地址 */
#define kern_vir_addr2phy_addr(x) ((unsigned long)(x))
#define kern_phy_addr2vir_addr(x) ((void *)((unsigned long)(x)))

unsigned long addr_vir2phy(unsigned long vaddr);

bool page_readable(unsigned long vaddr, unsigned long count);
bool page_writable(unsigned long vaddr, unsigned long nbytes);

int page_map_addr(unsigned long start, unsigned long len, unsigned long prot);
int page_unmap_addr(unsigned long vaddr, unsigned long len);

int page_map_addr_safe(unsigned long start, unsigned long len, unsigned long prot);
int page_unmap_addr_safe(unsigned long start, unsigned long len, char fixed);

int page_map_addr_fixed(unsigned long start, unsigned long addr, 
    unsigned long len, unsigned long prot);

unsigned long *kern_page_dir_copy_to();
#define kern_page_copy_storge               kern_page_dir_copy_to

void page_init();
void page_enable();

int do_map_pages(pgdir_t pgdir, uint64_t va, uint64_t size, uint64_t pa, int perm);
int do_map_pages_safe(pgdir_t pgdir, uint64_t va, uint64_t size, uint64_t pa, int perm);
void do_unmap_pages(pgdir_t pgdir, uint64_t va, uint64_t npages, int do_free);
void kern_mmap_early(uint64_t va, uint64_t pa, uint64_t sz, int perm);

void do_unmap_pages2(pgdir_t pgdir, uint64_t va, uint64_t npages, int do_free);

pte_t *page_walk(pgdir_t pgdir, uint64_t va, int alloc);
void pgdir_dump(pgdir_t pgdir, int level);

uint64_t *user_walk_addr(pgdir_t pgdir, uint64_t va);
/* 可以指定对某个页目录表进行操作 */
int page_map_addr_fixed2(pgdir_t pgdir, unsigned long start, unsigned long addr, 
    unsigned long len, unsigned long prot);
int page_unmap_addr2(pgdir_t pgdir, unsigned long vaddr, unsigned long len);
int page_unmap_addr_safe2(pgdir_t pgdir, unsigned long start, unsigned long len, char fixed);
int page_map_addr2(pgdir_t pgdir, unsigned long start, unsigned long len, unsigned long prot);
int page_map_addr_safe2(pgdir_t pgdir, unsigned long start, unsigned long len, unsigned long prot);

int page_copy_out(pgdir_t pgdir, uint64_t dstva, char *src, uint64_t len);
int page_copy_in(pgdir_t pgdir, char *dst, uint64_t srcva, uint64_t len);
int page_set(void *pgdir, uint64_t dstva, unsigned char val, uint64_t len);

#endif  /* _RISCV64_PAGE_H */
