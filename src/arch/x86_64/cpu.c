//
// Created by Lucas van Oosterhout on 2/18/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#include <stddef.h>
#include <stdio.h>
#include <kernel/cpu.h>
#include <stdbool.h>
#include "limine.h"

struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0,
};

void report_cpu(void) {
  if (smp_request.response == NULL)
    return;

  printf("Got %d cores\n", smp_request.response->cpu_count);
  printf("flags: %x\n", smp_request.response->flags);
  printf("lapic id: %x\n", smp_request.response->bsp_lapic_id);
  for (uint64_t i = 0; i < smp_request.response->cpu_count; i++) {
    struct limine_smp_info *cpu = smp_request.response->cpus[i];
    printf("-- cpu id: %x\n", cpu->processor_id);
    printf("   lapic id: %x\n", cpu->lapic_id);

    // goto_address - An atomic write to this field causes the parked CPU to jump to the written address,
    // on a 64KiB (or Stack Size Request size) stack. A pointer to the struct limine_smp_info structure of the CPU is
    // passed in RDI. Other than that, the CPU state will be the same as described for the bootstrap processor.
    // This field is unused for the structure describing the bootstrap processor.
    // For all CPUs, this field is guaranteed to be NULL when control is first passed to the bootstrap processor.
    printf("   goto addr: %x\n", &cpu->goto_address);
  }
}

// Halt and catch fire function. ToDo: Move to arch, this is x86 asm...
_Noreturn void hcf(void) {
  __asm__ ("cli");
  while (true) {
    __asm__ ("hlt");
  }
}
