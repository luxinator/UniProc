//
// Created by Lucas van Oosterhout on 5/31/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//
#include <string.h>
#include "proc.h"
#include "kernel.h"
#include "elf.h"

proc_entry proc_table[NR_PROC] = {};
proc_slot proc_current;
proc_slot proc_number;
int last_pid;

void create_proc0() {
  proc_table[0] = (proc_entry){
	  .context = create_new_proc_context(0, 0),
  };

  proc_entry *p = &proc_table[0];
}

proc_slot create_process(const uintptr_t start, const uintptr_t args) {
  if (proc_number >= NR_PROC) {
	panic("No more proc slots available");
	return 0;
  }

  proc_slot next_slot = 0;
  for (size_t i = 1; i < NR_PROC; i++) {
	if (proc_table[i].state==FREE) {
	  next_slot = i;
	  break;
	}
  }
  if (next_slot==0 && proc_table[0].state!=FREE) {
	panic("No more proc slots available");
	return 0;
  }

  proc_table[next_slot] = (proc_entry){
	  .context = create_new_proc_context(start, args),
	  .state = RUNNABLE,
	  .parent = 0,
  };

  proc_number++;
  return next_slot;
}

void name_process(proc_slot proc, char *name) {
  if (proc_table[proc].state!=FREE) {
	memcpy(&proc_table[proc].name, name, 16);
  }
}


