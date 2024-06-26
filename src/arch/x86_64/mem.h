//
// Created by Lucas van Oosterhout on 6/2/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#ifndef SRC_ARCH_X86_64_MEM_H_
#define SRC_ARCH_X86_64_MEM_H_
#include <kernel/vmem.h>

// Nasty since this is also defined in vmem.h
#define PAGE_SIZE 4096


#define PAGE_SHIFT 12
typedef struct page_fault {
  bool present;
  bool write;
  bool user;
  bool reserved;
  bool instruction_fetch;
  bool protection_key;
  bool shadow_stack;
  int flags;
} page_fault;

#define check_flag(flags, bit) ((flags) >> (bit)) & 1

page_fault paging_get_page_fault_flags(int error_code);

#endif //SRC_ARCH_X86_64_MEM_H_
