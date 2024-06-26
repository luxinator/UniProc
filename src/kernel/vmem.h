//
// Created by Lucas van Oosterhout on 2/14/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#ifndef SRC_ARCH_X86_64_VMEM_H_
#define SRC_ARCH_X86_64_VMEM_H_

#include <stdint.h>
#include <stddef.h>

#define MAX_MEMORY_REGIONS 16
#define PAGE_SIZE 4096

void report_memmap(void);
void build_memmap(void);
void report_kernel_address(void);
void print_size(uint64_t size);

uintptr_t kernel_start_physical(void);
uintptr_t kernel_start_virtual(void);

typedef struct memory_region {
  uintptr_t start;
  size_t length;
} memory_region;

extern memory_region memory_map[MAX_MEMORY_REGIONS];

// extremely simple page allocator,
//
// return non-zero on success, pages are PAGE_SIZE in size
void *get_free_page(void);
void free_page(void *);

#endif //SRC_ARCH_X86_64_VMEM_H_
