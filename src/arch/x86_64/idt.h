//
// Created by Lucas van Oosterhout on 2/13/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#ifndef SRC_ARCH_X86_64_IDT_H_
#define SRC_ARCH_X86_64_IDT_H_
#include <stdint.h>

typedef struct {
  uint64_t r15;
  uint64_t r14;
  uint64_t r13;
  uint64_t r12;
  uint64_t r11;
  uint64_t r10;
  uint64_t r9;
  uint64_t r8;
  uint64_t rsi;
  uint64_t rdi;
  uint64_t rbp;
  uint64_t rdx;
  uint64_t rcx;
  uint64_t rbx;
  uint64_t rax;
} __attribute__((packed)) cpu_state_t;

typedef struct {
  uint64_t isr_number;
  uint64_t error_code;
  uint64_t rip;
  uint64_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint64_t ss;
} __attribute__((packed)) stack_state_t;


void init_idt(void);

extern void interrupt_handler_0(void); //  ; divde by zero
extern void interrupt_handler_1(void); //  ; debug
extern void interrupt_handler_2(void); //  ; NMI external interrupt
extern void interrupt_handler_3(void); //  ; Breakpoint INT3 instruction
extern void interrupt_handler_4(void); //  ; Overflow
extern void interrupt_handler_5(void); //  ; Bound exceeds range
extern void interrupt_handler_6(void); //  ; Invalid Opcode
extern void interrupt_handler_7(void); //  ; Floating point exception, no Math Coprocessor
extern void error_code_handler_8(void); //  ; Double Fault
extern void interrupt_handler_9(void); //  ; CoProcessor Segment Overrun (reserved)
extern void error_code_handler_10(void); //   ; invalid TSS (Task switch or TSS access)
extern void error_code_handler_11(void); //   ; Segment not present
extern void error_code_handler_12(void); //   ; Stack Segement Fault
extern void error_code_handler_13(void); //   ; General Protection
extern void error_code_handler_14(void); //   ; Page Fault
extern void interrupt_handler_15(void); //   ;  Reserved
extern void interrupt_handler_16(void); //   ; Floating-Point Error (Math Fault)
extern void error_code_handler_17(void); //   ; Alignment Check
extern void interrupt_handler_18(void); //   ; Machine Check
extern void interrupt_handler_19(void); //   ; SIMD Floating-Point Exception
extern void interrupt_handler_20(void); //   ; Virtualization Exception
extern void interrupt_handler_21(void); //   ; Control Protection Exception
extern void interrupt_handler_22(void);
extern void interrupt_handler_23(void);
extern void interrupt_handler_24(void);
extern void interrupt_handler_25(void);
extern void interrupt_handler_26(void);
extern void interrupt_handler_27(void);
extern void interrupt_handler_28(void);
extern void interrupt_handler_29(void);
extern void interrupt_handler_30(void);
extern void interrupt_handler_31(void);

// IRQS
extern void interrupt_handler_32(void);
extern void interrupt_handler_33(void);
extern void interrupt_handler_34(void);
extern void interrupt_handler_35(void);
extern void interrupt_handler_36(void);
extern void interrupt_handler_37(void);
extern void interrupt_handler_38(void);
extern void interrupt_handler_39(void);
extern void interrupt_handler_40(void);
extern void interrupt_handler_41(void);
extern void interrupt_handler_42(void);
extern void interrupt_handler_43(void);
extern void interrupt_handler_44(void);
extern void interrupt_handler_45(void);
extern void interrupt_handler_46(void);
extern void interrupt_handler_47(void);
extern void interrupt_handler_48(void);

//
extern void load_idt(uint64_t);

#endif //SRC_ARCH_X86_64_IDT_H_
