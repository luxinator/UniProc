//
// Created by Lucas van Oosterhout on 7/7/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#include <kernel/timer.h>
#include "io.h"

#define IO_TIMER1       0x040           // 8253 Timer Channel 0
#define IO_TIMER3       0x043           // 8253 Timer Channel 2 (connected to speaker)

// Frequency of all three count-down timers;
// (TIMER_FREQ/freq) is the appropriate count
// to generate a frequency of freq Hz.

#define TIMER_FREQ      1193182
#define TIMER_DIV(x)    ((TIMER_FREQ+(x)/2)/(x))

#define TIMER_MODE      (IO_TIMER1 + 3) // timer mode port
#define TIMER_SEL0      0x00    // select counter 0
#define TIMER_RATEGEN   0x04    // mode 2, rate generator
#define TIMER_16BIT     0x30    // r/w counter 16 bits, LSB first

unsigned long ms_per_tick = 10;
volatile unsigned long timer_tick = 0;

void timerinit(void) {
  // Interrupt 100 times/sec.
  outb(TIMER_MODE, TIMER_SEL0 | TIMER_RATEGEN | TIMER_16BIT);
  outb(IO_TIMER1, TIMER_DIV(100)%256);
  outb(IO_TIMER1, TIMER_DIV(100)/256);
}

void sleep(unsigned int ms) {
  if (ms==0)
	return;

  const unsigned int stop_at = timer_tick + ms/ms_per_tick;
  while (true) {
	if (timer_tick > stop_at)
	  break;
	__asm__("hlt");
  }
}