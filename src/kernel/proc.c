//
// Created by Lucas van Oosterhout on 5/31/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "proc.h"
#include "kernel.h"

proc_entry proc_table[NR_PROC] = {};
size_t proc_current;
size_t proc_next;
size_t proc_number;
int last_pid;

bool running = false;

proc_slot fork(int parent) {
}

void create_proc0() {
  proc_table[0] = (proc_entry) {
      .priority = 6,
      .counter = 6,
      .preempt_count = 0,
      .pid = last_pid++,
      .parent = 0,
      .context = create_new_proc_context(0, 0),
      .state = SLEEP,
  };

  proc_entry *p = &proc_table[0];

}

proc_slot create_process(uintptr_t start, uintptr_t args) {
  // Make sure we are never preempted
  preempt_disable();

  if (proc_number >= NR_PROC) {
    panic("No more proc slots available");
    return 0;
  }

  proc_slot next_slot = 0;
  for (size_t i = 0; i < NR_PROC; i++) {
    if (proc_table[i].state == FREE) {
      next_slot = i;
      break;
    }
  }
  if (next_slot == 0 && proc_table[0].state != FREE) {
    panic("No more proc slots available");
    return 0;
  }

  // ToDo: set priority
  proc_table[next_slot] = (proc_entry) {
      .priority = 6,
      .counter = 6,
      .preempt_count = 0,
      .pid = last_pid++,
      .parent = 0,
      .context = create_new_proc_context(start, args),
      .state = SLEEP,
  };

  proc_entry new_proc = proc_table[next_slot];

  // -- Sort proc_table on prio
  // We could of course split the proc_table into a list of indices (or pointers) and sort the list instead of the table
  // This is quite expansive and freeing is too,
  // This is why linked (priority) lists are used in Unix/Linux/Xinu
  for (size_t i = 0; i < proc_number; i++) {
    if (proc_table[i].priority != 0 && proc_table[i].priority < new_proc.priority) {
      // Copy all entries one lower
      for (size_t j = proc_number - 1; j > i; j--) {
        memcpy(&proc_table[j], &proc_table[j - 1], sizeof(proc_entry));
      }
      memcpy(&proc_table[i], &new_proc, sizeof(proc_entry));
      next_slot = i;
      break;
    }
  }

  proc_number++;

  printf("pro table\n");
  for (size_t i = 0; i < proc_number; ++i) {
    if (i == proc_current)
      printf("!");
    printf("  proc: %d\n", proc_table[i].pid);
    printf("  cntx: %x\n", proc_table[i].context);
  }

  preempt_enable();

  return
      next_slot;
}

proc_slot next_proc(void) {
// finding next is easy
  proc_slot next = proc_current + 1;
  if (next > NR_PROC)
    panic("Proc table is full");
  return next;
}

inline void preempt_disable(void) {
  proc_table[proc_current].preempt_count += 1;
};

void preempt_enable(void) {
  proc_table[proc_current].preempt_count -= 1;
}