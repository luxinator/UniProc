//
// Created by Lucas van Oosterhout on 5/31/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "vmem.h"
#include "kernel.h"
#include "limine.h"

// todo: create the bitmap in a usable region, keep it out of the kernel binary
#define PAGE_TABLE_SIZE 4096
// 4096 * 4096 bytes -> 16Mb heap
#define TOTAL_HEAP_SIZE PAGE_TABLE_SIZE * PAGE_SIZE

struct limine_hhdm_request limine_hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

typedef struct page_table {
  bool pages[PAGE_TABLE_SIZE];
} page_table;

uintptr_t base_of_heap = 0;
page_table pageTable = {};

void *get_free_page(void) {
  if (base_of_heap == 0) {
    uintptr_t hhdm_offset = limine_hhdm_request.response->offset;

    // todo: create the bitmap in a mem region, keep it out of the kernel binary
    // this also create bitmaps of variable sizes

    // find first mem region to fit a heap
    for (size_t i = 0; i < MAX_MEMORY_REGIONS; i++) {
      if (memory_map[i].length >= TOTAL_HEAP_SIZE) {
        base_of_heap = memory_map[i].start + hhdm_offset;
        printf("Creating heap at %x with %d pages\n", base_of_heap, memory_map[i].length / PAGE_SIZE);
        break;
      }
    }
    if (base_of_heap == 0) {
      panic("No memory region big enough to fit heap");
    }
  }

  for (size_t i = 0; i < PAGE_TABLE_SIZE; i++) {
    if (pageTable.pages[i] == false) {
      pageTable.pages[i] = true;
      return (uint8_t *) (base_of_heap + PAGE_SIZE * i);
    }
  }
    panic("Heap is full!");

  return 0;
}

void free_page(void *page) {
  uintptr_t virtual = (uintptr_t) page + kernel_start_physical();
  size_t i = (virtual - base_of_heap) / PAGE_SIZE;
  pageTable.pages[i] = false;
}