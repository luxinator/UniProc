//
// Created by Lucas van Oosterhout on 5/31/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//
#include <stdio.h>
#include "kernel.h"
#include "proc.h"

unsigned long timer_ticks = 0;

void schedule(void) {
  // Round robin scheduler

  proc_slot old = proc_current;
  proc_slot next = next_proc();
  // Current proc is out of time, pick the next one.

  if (next >= proc_number) {
    // Set all tasks counters back to their prio
    for (size_t i = 0; i < proc_number; i++) {
      proc_table[i].counter = proc_table[i].priority;
    }
    next = 0;
  }

  if (proc_current == next)
    return;

  printf("Picked next proc: %d\n", next);
  proc_current = next;
  switch_context(proc_table[proc_current].context, proc_table[old].context);

  preempt_enable();
}

void schedule_tick(void) {
  timer_ticks++;
  proc_entry *current = &proc_table[proc_number];
  current->counter -= 1;
  // Proc has time left or cannot be preempted
  if (current->counter > 0 || current->preempt_count > 0) {
    return;
  }

  current->counter = 0;
  schedule();
  disable_interrupts();
}