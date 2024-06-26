//
// Created by Lucas van Oosterhout on 2/19/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#include <kernel/proc.h>
#include <kernel/kernel.h>
#include <stdio.h>
#include <string.h>
#include "idt.h"
#include "kernel/vmem.h"
#include "proc_context.h"

void _start(void (*pro_entry_c)(char *), char *args) {
  printf("Jumping\n");
  pro_entry_c(args);
};

extern void _switch_context(struct proc_context *new);

typedef void func(char *ar);

void switch_context(struct proc_context *new, struct proc_context *old) {
//  printf("from rsp: %x, rpi: %x\n", old->stack_state.rsp, old->stack_state.rip);
//  printf("switching to rsp: %x, rpi: %x\n", new->stack_state.rsp, new->stack_state.rip);

//    func* f = (func*)new->cpu_state.rdi;
//    char* args = (char*)new->cpu_state.rsi;
//    (*f)(args);


  _switch_context(new);
}

struct proc_context *create_new_proc_context(uintptr_t start, uintptr_t args_ptr) {
  // This should be a KERNEL page.
  void *page = get_free_page();

  // Zero out page
  uint8_t *byte_ptr = (uint8_t *) (page);
  memset(byte_ptr, 0, PAGE_SIZE);

  struct proc_context *cntx = (struct proc_context *) page;
  // Instruction pointer
  cntx->stack_state.rip = (uint64_t) &_start;

  // System V calling convention:
  // arg 1 -> rdi
  // arg 2 -> rsi
  // arg 3 -> rdx
  // arg 4 -> rcx
  // arg 5 -> r8
  // arg 6 -> r9
  // args 7+ -> onto the stack
  cntx->cpu_state.rdi = start;
  cntx->cpu_state.rsi = args_ptr;

  // ToDo: putting the stack and heap on same page is not really cool
  // this should be some user space page
  cntx->stack_state.rsp = (uint64_t) (page + PAGE_SIZE);
  return cntx;
}