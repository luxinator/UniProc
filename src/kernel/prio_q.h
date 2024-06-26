//
// Created by Lucas van Oosterhout on 2/19/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#ifndef SRC_KERNEL_PRIO_Q_H_
#define SRC_KERNEL_PRIO_Q_H_

#include <stdbool.h>
#include <stddef.h>

#define QUEUE_SIZE  4096

typedef struct {
  int prio;
  struct queue_entry *next;
  struct queue_entry *prev;
} queue_entry;

queue_entry prio_queue[QUEUE_SIZE];
size_t next_in_queue;

inline bool is_empty() {
  return next_in_queue == 0;
}

inline bool non_empty() {
  return !is_empty();
}

int enqueue(int item);
int dequeue(int item);
int q_next();

#endif //SRC_KERNEL_PRIO_Q_H_

