//
// Created by Lucas van Oosterhout on 2/18/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//


#include "pic_8259.h"
#include "io.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */

#define ICW1_ICW4    0x01        /* Indicates that ICW4 will be present */
#define ICW1_SINGLE    0x02        /* Single (cascade) mode */
#define ICW1_INTERVAL4    0x04        /* Call address interval 4 (8) */
#define ICW1_LEVEL    0x08        /* Level triggered (edge) mode */
#define ICW1_INIT    0x10        /* Initialization - required! */

#define ICW4_8086    0x01        /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO    0x02        /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE    0x08        /* Buffered mode/slave */
#define ICW4_BUF_MASTER    0x0C        /* Buffered mode/master */
#define ICW4_SFNM    0x10        /* Special fully nested (not) */

#define PIC_READ_IRR                0x0a    /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR                0x0b    /* OCW3 irq service next CMD read */

/*
arguments:
	offset1 - vector offset for master PIC
		vectors on the master become offset1..offset1+7
	offset2 - same for slave PIC: offset2..offset2+7
*/
void pic_remap(uint8_t offset1, uint8_t offset2) {
  uint8_t a1, a2;

  a1 = inb(PIC1_DATA);                        // save masks
  a2 = inb(PIC2_DATA);

  outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
  io_wait();
  outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
  io_wait();
  outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
  io_wait();
  outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
  io_wait();
  outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
  io_wait();
  outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
  io_wait();

  outb(PIC1_DATA, ICW4_8086);               // ICW4: have the PICs use 8086 mode (and not 8080 mode)
  io_wait();
  outb(PIC2_DATA, ICW4_8086);
  io_wait();

  outb(PIC1_DATA, a1);   // restore saved masks.
  outb(PIC2_DATA, a2);
}

void pic_send_eoi(uint8_t irq) {
  if (irq >= 8)
    outb(PIC2_COMMAND, PIC_EOI);

  outb(PIC1_COMMAND, PIC_EOI);
}

void pic_disable_all(void) {
  outb(PIC1_DATA, 0xff);
  outb(PIC2_DATA, 0xff);
}

void irq_set_mask(uint8_t IRQline) {
  uint16_t port;
  uint8_t value;

  if (IRQline < 8) {
    port = PIC1_DATA;
  } else {
    port = PIC2_DATA;
    IRQline -= 8;
  }
  value = inb(port) | (1 << IRQline);
  io_wait();

  outb(port, value);
  io_wait();
}

void irq_clear_mask(uint8_t irq_line) {
  uint16_t port;

  if (irq_line < 8) {
    port = PIC1_DATA;
  } else {
    port = PIC2_DATA;
    irq_line -= 8;
  }
  uint8_t value = inb(port);
  value = value & ~(1 << irq_line);
  io_wait();

  outb(port, value);
  io_wait();
}

/* Helper func */
static uint16_t pic_get_irq_reg(int ocw3) {
  /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
   * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
  outb(PIC1_COMMAND, ocw3);
  outb(PIC2_COMMAND, ocw3);
  return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

/* Returns the combined value of the cascaded PICs irq request register */
uint16_t pic_get_irr(void) {
  return pic_get_irq_reg(PIC_READ_IRR);
}

/* Returns the combined value of the cascaded PICs in-service register */
uint16_t pic_get_isr(void) {
  return pic_get_irq_reg(PIC_READ_ISR);
}

uint16_t pic_get_mask(void) {
  uint16_t value = inb(PIC1_DATA) << 8 | inb(PIC2_DATA);
  return value;
}
