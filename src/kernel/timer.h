//
// Created by Lucas van Oosterhout on 7/7/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#ifndef UNIPROC_SRC_KERNEL_TIMER_H_
#define UNIPROC_SRC_KERNEL_TIMER_H_

extern volatile unsigned long timer_tick;
extern unsigned long ms_per_tick;

void timerinit(void);
void sleep(unsigned int ms);

#endif //UNIPROC_SRC_KERNEL_TIMER_H_
