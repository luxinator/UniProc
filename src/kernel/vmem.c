//
// Created by Lucas van Oosterhout on 2/14/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#include <stdio.h>
#include <limine.h>
#include "vmem.h"

memory_region memory_map[MAX_MEMORY_REGIONS];

struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0,
};

void print_size(uint64_t size) {
  if (size > 0) {
    if (size > 1024 * 1024) {
      printf("%d Mb", size / 1024 / 1024);
    } else if (size > 1024) {
      printf("%d Kb", size / 1024);
    } else
      printf("%d bytes", size);
  } else {
    printf("0 bytes");
  }
}

void build_memmap(void) {
  int region_counter = 0;

  for (uint64_t i = 0; i < memmap_request.response->entry_count; i++) {
    struct limine_memmap_entry *entry = memmap_request.response->entries[i];
    if (entry->type == LIMINE_MEMMAP_USABLE) {
      if (region_counter < MAX_MEMORY_REGIONS) {
        memory_map[region_counter].start = entry->base;
        memory_map[region_counter].length = entry->length;
        region_counter++;
      }
    }
  }
}

void report_memmap(void) {
  if (memmap_request.response == NULL) {
    printf("LIMINE Memory map not provided!");
    return;
  }

  printf("Physical Memory regions:\n");

  for (uint64_t i = 0; i < memmap_request.response->entry_count; i++) {
    struct limine_memmap_entry *entry = memmap_request.response->entries[i];

    printf("BASE: %x - %x ", entry->base, entry->base + entry->length);
    print_size(entry->length);
    switch (entry->type) {
      case (LIMINE_MEMMAP_USABLE):printf(" USABLE");
        break;
      case (LIMINE_MEMMAP_RESERVED):printf(" RESERVED");
        break;
      case (LIMINE_MEMMAP_ACPI_RECLAIMABLE):printf(" ACPI_RECLAIMABLE");
        break;
      case (LIMINE_MEMMAP_ACPI_NVS):printf(" ACPI_NVS");
        break;
      case (LIMINE_MEMMAP_BAD_MEMORY):printf(" BAD_MEMORY!");
        break;
      case (LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE):printf(" BOOTLOADER_RECLAIMABLE");
        break;
      case (LIMINE_MEMMAP_KERNEL_AND_MODULES):printf(" KERNEL_AND_MODULES");
        break;
      case (LIMINE_MEMMAP_FRAMEBUFFER):printf(" FRAMEBUFFER");
        break;
    }
    if (entry->base < 0x100000)
      printf(" (LOW RAM) ");

    printf("\n");
  }
}

void report_kernel_address(void) {
  if (kernel_address_request.response == NULL) {
    printf("LIMINE kernel address not found");
    return;
  }

  printf("Kernel is at: %x physical - %x virtual\n",
         kernel_address_request.response->physical_base,
         kernel_address_request.response->virtual_base);
}

uintptr_t kernel_start_physical(void) {
  return kernel_address_request.response->physical_base;
}

uintptr_t kernel_start_virtual(void) {
  return kernel_address_request.response->virtual_base;
}



