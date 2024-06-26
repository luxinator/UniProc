//
// Created by Lucas van Oosterhout on 2/19/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#ifndef SRC_KERNEL_PROC_H_
#define SRC_KERNEL_PROC_H_

#include <stddef.h>
#include <stdint.h>

#define NR_PROC 256
typedef size_t proc_slot;

typedef enum proc_state {
  FREE = 0,
  RUNNING,
  RUNNABLE,
  SLEEP,
  ZOMBIE = 255
} proc_state;

#define PRC_NAME_LEN   16

// This struct is defined here, but actually created in arch/{arch}/proc_context.c
struct proc_context;
struct proc_context *create_new_proc_context(uintptr_t first_instruction, uintptr_t args);

/**
 * Much like in Linux a proc (called a task in Linux) has a prio and a counter.
 * Every time the timer interrupts, we decrement the counter.
 * When the counter reaches zero we switch the task with the next highest prio.
 * When no tasks remains we start with the highest prio and start again.
 * This is a RoundRobin scheduler.
 */
typedef struct proc_entry {
  int priority;
  int counter;
  // When this is non-zero the timer interrupt should just return. This means the proc is rescheduling.
  int preempt_count;
  // For convenience
  char name[PRC_NAME_LEN];
  int pid;
  int parent;

  struct proc_context *context;
  proc_state state;
} proc_entry;

extern proc_entry proc_table[NR_PROC];
extern size_t proc_current;
extern size_t proc_next;
extern size_t proc_number;
extern int last_pid;

extern void switch_context(struct proc_context *new, struct proc_context *old);

// create a sleeping process with the IP at start and args as a pointer to the stack
proc_slot create_process(uintptr_t start, uintptr_t args);

void create_proc0();

// forks a process, copying all the contents of the parent and putting it in SLEEP
proc_slot fork(int parent);

// Table is sorted on prio, so finding the next is easy

proc_slot next_proc(void);

void preempt_disable(void);
void preempt_enable(void);

#endif //SRC_KERNEL_PROC_H_
