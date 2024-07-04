//
// Created by Lucas van Oosterhout on 2/19/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#ifndef SRC_KERNEL_PROC_H_
#define SRC_KERNEL_PROC_H_

#include <stddef.h>
#include <stdint.h>

#define NR_PROC 32
typedef size_t proc_slot;

typedef enum proc_state {
  FREE = 0,
  RUNNING,
  RUNNABLE,
  SLEEP,
  ZOMBIE = 255
} proc_state;

#define PRC_NAME_LEN   16

// This struct is defined here, but implemented in arch/{arch}/proc_context.c
struct proc_context;

/**
 * @param first_instruction when we jump to this address the process starts
 * @param args pointer to the arguments
 * @return
 */
struct proc_context *create_new_proc_context(uintptr_t first_instruction, uintptr_t args);

typedef struct proc_entry {
  char name[PRC_NAME_LEN];
  int parent;
  struct proc_context *context;
  proc_state  state;
} proc_entry;

extern proc_entry proc_table[NR_PROC];
extern size_t proc_current;
extern int last_pid;

/**
 * @param start when we jump to this address the process starts
 * @param args pointer to the arguments
 * @return non-zero on success
 */
proc_slot create_process(uintptr_t start, uintptr_t args);
void name_process(proc_slot proc, char* name);

void create_proc0();

#endif //SRC_KERNEL_PROC_H_
