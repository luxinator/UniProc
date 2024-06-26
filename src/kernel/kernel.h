//
// Created by Lucas van Oosterhout on 4/10/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#ifndef SRC_KERNEL_KERNEL_H_
#define SRC_KERNEL_KERNEL_H_

void enable_interrupts(void);
void disable_interrupts(void);
void schedule_tick(void);
void schedule(void);
void schedule_tick(void);

void enter_panic(char* file, int line, char* message);
void init_virtual_memory(void);

#define panic(msg) enter_panic(__FILE__,__LINE__, #msg);


#endif //SRC_KERNEL_KERNEL_H_
