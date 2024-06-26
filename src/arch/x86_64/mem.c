//
// Created by Lucas van Oosterhout on 2/14/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "kernel/vmem.h"
#include "mem.h"

extern void load_page_table(uintptr_t base);
extern void flush_tlb(void);
extern uintptr_t report_cr3();

typedef struct page_table_entry {
  bool present: 1;
  bool readwrite: 1;
  bool supervisor: 1;
  bool writethrough: 1;
  bool cache_disabled: 1;
  bool accessed: 1;
  bool dirty: 1;
  bool pagesize: 1;
  bool global: 1;
  uint8_t avail: 3;
  uint64_t address: 52;
} __attribute__((packed)) page_table_entry;

typedef struct page_table {
  page_table_entry entries[512];
} __attribute__((packed)) page_table;

// https://zolutal.github.io/understanding-paging/
// https://os.phil-opp.com/paging-introduction/
// Page Table (Lowest)
page_table lvl1_page_table = {};
// Page Middle Directory
page_table lvl2_page_table = {};
// Page Upper Directory
page_table lvl3_page_table = {};
// Page Global Directory -> goes into CR3
page_table lvl4_page_table = {};

page_table_entry create_entry(uintptr_t paddr, int flags) {
  return (page_table_entry) {
      .present = check_flag(flags, 0),
      .readwrite = check_flag(flags, 1),
      .supervisor = check_flag(flags, 2),
      .writethrough = 0,
      .cache_disabled = 0,
      .accessed = 0,
      .dirty = 0,
      .pagesize = 0, // PS/PAT bit
      .global = 0, // Todo: set this
      .avail = 0,
      .address = paddr >> PAGE_SHIFT // PFN
  };
}

page_table_entry paging_purge_entry(void) {
  return create_entry(0, 0);
}

page_fault paging_get_page_fault_flags(int error_code) {
  bool write = check_flag(error_code, 1);
  bool user = check_flag(error_code, 2);
  bool ins_fetch = check_flag(error_code, 4);
  bool prot_key = check_flag(error_code, 5);
  bool shadow_stack = check_flag(error_code, 6);

  return (page_fault)
      {
          .present = 1,
          .write = write,
          .user = user,
          .reserved = 0,
          .instruction_fetch = ins_fetch,
          .protection_key = prot_key,
          .shadow_stack = shadow_stack,
          .flags = (user << 2) | (write << 1) | (1 << 0)
      };
}

void init_virtual_memory(void) {


//  uintptr_t cr3 = report_cr3();
//  printf("Page Global Dir (cr3) at: %x\n", cr3);
//  page_table *current = (page_table *) (kernel_start_physical() - cr3);
//  for (size_t i = 0; i < 512; i++) {
//    printf("%d   %x\n", i, current->entries[i]);
//  }

}