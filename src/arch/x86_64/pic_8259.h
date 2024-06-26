//
// Created by Lucas van Oosterhout on 2/18/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#ifndef SRC_ARCH_X86_64_PIC_8259_H_
#define SRC_ARCH_X86_64_PIC_8259_H_

#include <stdint.h>

// Code based on: https://wiki.osdev.org/PIC
// Lots of old x86 stuf can be found here: https://helppc.netcore2k.net/hardware/pic

#define PIC1          0x20        /* IO base address for master PIC */
#define PIC2          0xA0        /* IO base address for slave PIC */
#define PIC1_COMMAND  PIC1
#define PIC1_DATA    (PIC1+1)
#define PIC2_COMMAND  PIC2
#define PIC2_DATA    (PIC2+1)
#define PIC_EOI       0x20        /* End-of-interrupt command code */

#define TIMER 0
#define KEYBOARD_IRQ 1
#define COM_1 4
#define COM_2 3
#define FLOPPY 6
#define RTC 8
#define IDE_BUS_1 14
#define IDE_BUS_2 15

void pic_send_eoi(uint8_t irq);
void pic_disable_all(void);
void pic_remap(uint8_t offset1, uint8_t offset2);
void irq_set_mask(uint8_t IRQline);
void irq_clear_mask(uint8_t irq_line);

/* Returns the combined value of the cascaded PICs irq request register */
uint16_t pic_get_irr(void);

/* Returns the combined value of the cascaded PICs in-service register */
uint16_t pic_get_isr(void);

uint16_t pic_get_mask(void);
#endif //SRC_ARCH_X86_64_PIC_8259_H_
