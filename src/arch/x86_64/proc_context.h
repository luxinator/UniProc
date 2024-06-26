//
// Created by lucas on 6/7/24.
//

#ifndef PROC_CONTEXT_H
#define PROC_CONTEXT_H

struct proc_context {
    cpu_state_t cpu_state;
    stack_state_t stack_state;
}__attribute__((packed));

#endif //PROC_CONTEXT_H
