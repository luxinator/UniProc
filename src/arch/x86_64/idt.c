//
// Created by Lucas van Oosterhout on 2/13/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#include "idt.h"
#include <stdio.h>

typedef struct {
  uint16_t isr_low;      // The lower 16 bits of the ISR's address
  uint16_t kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
  uint8_t ist;          // The IST in the TSS that the CPU will load into RSP; set to zero for now
  uint8_t attributes;   // Type and attributes; see the IDT page
  uint16_t isr_mid;      // The higher 16 bits of the lower 32 bits of the ISR's address
  uint32_t isr_high;     // The higher 32 bits of the ISR's address
  uint32_t reserved;     // Set to zero
} __attribute__((packed)) idt_entry_t;

__attribute__((aligned(0x10))) idt_entry_t idt[256] = {0}; // Create an array of IDT entries; aligned for performance


typedef struct {
  uint16_t limit;
  uintptr_t base;
} __attribute__((packed)) idtr_t;

static idtr_t idtr;

/**
 * Helper function to populate the IDT
 * @param index number of the handler
 * @param isr offset
 * @param flags
 */
void idt_set_descriptor(uint8_t index, uint64_t isr, uint8_t flags) {
  idt[index].isr_low = (uint64_t) isr & 0xFFFF;
  idt[index].kernel_cs = 0x08;  // CS of kernel, limine puts zero here
  idt[index].ist = 0;
  idt[index].attributes = flags;
  idt[index].isr_mid = ((uint64_t) isr >> 16) & 0xFFFF;
  idt[index].isr_high = ((uint64_t) isr >> 32) & 0xFFFFFFFF;
  idt[index].reserved = 0;
}

void init_idt(void) {
  idt_set_descriptor(0, (uint64_t) interrupt_handler_0, 0x8e); //  ; divde by zero
  idt_set_descriptor(1, (uint64_t) interrupt_handler_1, 0x8e); //  ; debug
  idt_set_descriptor(2, (uint64_t) interrupt_handler_2, 0x8e); //  ; NMI external interrupt
  idt_set_descriptor(3, (uint64_t) interrupt_handler_3, 0x8e); //  ; Breakpoint INT3 instruction
  idt_set_descriptor(4, (uint64_t) interrupt_handler_4, 0x8e); //  ; Overflow
  idt_set_descriptor(5, (uint64_t) interrupt_handler_5, 0x8e); //  ; Bound exceeds range
  idt_set_descriptor(6, (uint64_t) interrupt_handler_6, 0x8e); //  ; Invalid Opcode
  idt_set_descriptor(7, (uint64_t) interrupt_handler_7, 0x8e); //  ; Floating point exception, no Math Coprocessor
  idt_set_descriptor(8, (uint64_t) error_code_handler_8, 0x8e); //  ; Double Fault
  idt_set_descriptor(9, (uint64_t) interrupt_handler_9, 0x8e); //  ; CoProcessor Segment Overrun (reserved)
  idt_set_descriptor(10, (uint64_t) error_code_handler_10, 0x8e); //   ; invalid TSS (Task switch or TSS access)
  idt_set_descriptor(11, (uint64_t) error_code_handler_11, 0x8e); //   ; Segment not present
  idt_set_descriptor(12, (uint64_t) error_code_handler_12, 0x8e); //   ; Stack Segement Fault
  idt_set_descriptor(13, (uint64_t) error_code_handler_13, 0x8e); //   ; General Protection
  idt_set_descriptor(14, (uint64_t) error_code_handler_14, 0x8e); //   ; Page Fault
  idt_set_descriptor(15, (uint64_t) interrupt_handler_15, 0x8e); //   ;  Reserved
  idt_set_descriptor(16, (uint64_t) interrupt_handler_16, 0x8e); //   ; Floating-Point Error (Math Fault)
  idt_set_descriptor(17, (uint64_t) error_code_handler_17, 0x8e); //   ; Alignment Check
  idt_set_descriptor(18, (uint64_t) interrupt_handler_18, 0x8e); //   ; Machine Check
  idt_set_descriptor(19, (uint64_t) interrupt_handler_19, 0x8e); //   ; SIMD Floating-Point Exception
  idt_set_descriptor(20, (uint64_t) interrupt_handler_20, 0x8e); //   ; Virtualization Exception
  idt_set_descriptor(21, (uint64_t) interrupt_handler_21, 0x8e); //   ; Control Protection Exception

  idt_set_descriptor(22, (uint64_t) interrupt_handler_22, 0x8e);
  idt_set_descriptor(23, (uint64_t) interrupt_handler_23, 0x8e);
  idt_set_descriptor(24, (uint64_t) interrupt_handler_24, 0x8e);
  idt_set_descriptor(25, (uint64_t) interrupt_handler_25, 0x8e);
  idt_set_descriptor(26, (uint64_t) interrupt_handler_26, 0x8e);
  idt_set_descriptor(27, (uint64_t) interrupt_handler_27, 0x8e);
  idt_set_descriptor(28, (uint64_t) interrupt_handler_28, 0x8e);
  idt_set_descriptor(29, (uint64_t) interrupt_handler_29, 0x8e);
  idt_set_descriptor(30, (uint64_t) interrupt_handler_30, 0x8e);
  idt_set_descriptor(31, (uint64_t) interrupt_handler_31, 0x8e);
// IRQs

  idt_set_descriptor(32, (uint64_t) interrupt_handler_32, 0x8e);
  idt_set_descriptor(33, (uint64_t) interrupt_handler_33, 0x8e);
  idt_set_descriptor(34, (uint64_t) interrupt_handler_34, 0x8e);
  idt_set_descriptor(35, (uint64_t) interrupt_handler_35, 0x8e);
  idt_set_descriptor(36, (uint64_t) interrupt_handler_36, 0x8e);
  idt_set_descriptor(37, (uint64_t) interrupt_handler_37, 0x8e);
  idt_set_descriptor(38, (uint64_t) interrupt_handler_38, 0x8e);
  idt_set_descriptor(39, (uint64_t) interrupt_handler_39, 0x8e);
  idt_set_descriptor(40, (uint64_t) interrupt_handler_40, 0x8e);
  idt_set_descriptor(41, (uint64_t) interrupt_handler_41, 0x8e);
  idt_set_descriptor(42, (uint64_t) interrupt_handler_42, 0x8e);
  idt_set_descriptor(43, (uint64_t) interrupt_handler_43, 0x8e);
  idt_set_descriptor(44, (uint64_t) interrupt_handler_44, 0x8e);
  idt_set_descriptor(45, (uint64_t) interrupt_handler_45, 0x8e);
  idt_set_descriptor(46, (uint64_t) interrupt_handler_46, 0x8e);
  idt_set_descriptor(47, (uint64_t) interrupt_handler_47, 0x8e);
  idt_set_descriptor(48, (uint64_t) interrupt_handler_48, 0x8e);

  idtr = (idtr_t) {
      .base = (uint64_t) &idt,
      .limit = 256 * sizeof(idt_entry_t) - 1,
  };

  load_idt((uint64_t) &idtr);
}
