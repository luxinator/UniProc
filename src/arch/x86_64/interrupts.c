//
// Created by Lucas van Oosterhout on 2/18/24.
// Copyright (c) 2024 Lucas van Oosterhout All rights reserved.
//

#include <kernel/interrupts.h>
#include <kernel/kernel.h>
#include <stdbool.h>
#include <stdio.h>

#include <stdlib.h>
#include "gdt.h"
#include "idt.h"
#include "pic_8259.h"
#include "cpuid.h"
#include "keyboard.h"
#include "mem.h"
#include "kernel/proc.h"
#include "proc_context.h"
#include "kernel/timer.h"

extern uintptr_t read_cr2(void);

/** returns a 'true' value if the CPU supports APIC
 *  and if the local APIC hasn't been disabled in MSRs
 *  note that this requires CPUID to be supported.
 */
bool check_apic() {
  uint32_t eax, edx;
  cpuid(1, &eax, &edx);
  return edx & CPUID_FEAT_EDX_APIC;
}

void enable_interrupts(void) {
  __asm__("sti");
};

void disable_interrupts(void) {
  __asm__("cli");
}

void initialize_interrupts(void) {
  init_gdt();
  init_idt();

  timerinit();
  // Map PIC IRQs to interrupts 32 to 47
  pic_remap(0x20, 0x28);

  // flush all pending
  for (uint8_t i = 0; i < 16; i++) {
	pic_send_eoi(i);
  }

  irq_clear_mask(TIMER);
  irq_clear_mask(KEYBOARD_IRQ);
}

void int_enable_keyboard(void) {
  irq_clear_mask(KEYBOARD_IRQ);
}

void debug_cpu_dump(cpu_state_t *registers, stack_state_t *stack) {
  printf("--- General Registers ---\n");
  printf("r15: %x\n", registers->r15);
  printf("r14: %x\n", registers->r14);
  printf("r13: %x\n", registers->r13);
  printf("r12: %x\n", registers->r12);
  printf("r11: %x\n", registers->r11);
  printf("r10: %x\n", registers->r10);
  printf("r9: %x\n", registers->r9);
  printf("r8: %x\n", registers->r8);
  printf("rsi: %x\n", registers->rsi);
  printf("rdi: %x\n", registers->rdi);
  printf("rbp: %x\n", registers->rbp);
  printf("rdx: %x\n", registers->rdx);
  printf("rcx: %x\n", registers->rcx);
  printf("rbx: %x\n", registers->rbx);
  printf("rax: %x\n", registers->rax);

  printf("isr_number: (%d) %x\n", stack->isr_number, stack->isr_number);
  printf("error_code: (%d) %x \n", stack->error_code, stack->error_code);
  printf("rip: %x\n", stack->rip);
  printf("cs: %x\n", stack->cs);
  printf("rflags: %b\n", stack->rflags);
  printf("rsp: %x\n", stack->rsp);
  printf("ss: %x\n", stack->ss);
}

extern void generic_handler(cpu_state_t registers, stack_state_t stack) {

//  proc_table[proc_current].context->cpu_state = registers;
//  proc_table[proc_current].context->stack_state = stack;

  // SYSCALL
  if (stack.isr_number==64) {
	// Do something like in Xinu:
	// if(proc->killed)
	//   exit();
	// proc->tf = tf;
	// syscall();
	// if(proc->killed)
	//   exit();
	// return;
  }

// IRQ
  if (stack.isr_number >= 32 && stack.isr_number < 48) {
	switch (stack.isr_number) {
	  case 33: {
		int s = kb_get_scancode();
		// ToDo this needs to go to Console
		printf("%c", (char)s);
	  }
		break;

	  case 32: {
		timer_tick += 1;
		break;
	  }
	  default: {
		printf("UNKOWN IRQ: [%d]! ISR: %x IRR: %x\n", stack.isr_number, pic_get_isr(), pic_get_irr());
		debug_cpu_dump(&registers, &stack);
		break;
	  }
	}

	// ACK the interrupt
	pic_send_eoi(stack.error_code);
	return;
  }

  debug_cpu_dump(&registers, &stack);
  switch (stack.isr_number) {
	case (0) :panic("INT FAULT: divde by zero");
	  break;
	case (1) :panic("INT FAULT: debug");
	  break;
	case (2) :panic("INT FAULT:   ; NMI external interrupt");
	  break;
	case (3) : {
	  panic("INT FAULT: Breakpoint INT3 instruction");
	  return;
	}
	case (4) :panic("INT FAULT: Overflow");
	  break;
	case (5) :panic("INT FAULT:   ; Bound exceeds range");
	  break;
	case (6) :panic("INT FAULT: Invalid Opcode");
	  break;
	case (7) :panic("INT FAULT: Floating point exception, no Math Coprocessor");
	  break;
	case (8) :panic("INT FAULT: Double Fault");
	  break;
	case (9) :panic("INT FAULT: CoProcessor Segment Overrun (reserved)");
	  break;
	case (10) :panic("INT FAULT: invalid TSS (Task switch or TSS access)");
	  break;
	case (11) :panic("INT FAULT: Segment not present");
	  break;
	case (12) :panic("INT FAULT: Stack Segement Fault");
	  break;
	case (13) :panic("INT FAULT: General Protection");
	  break;
	case (14) : {
	  printf("\n!!! PAGE FAULT !!!:\n");
	  uintptr_t at_addr = read_cr2();
	  printf("at: %x\n", at_addr);
	  page_fault fault = paging_get_page_fault_flags(stack.error_code);
	  printf("present: %b\n"
			 "write: %b\n"
			 "user: %b\n"
			 "reserved: %b\n"
			 "instruction_fetch: %b\n"
			 "protection_key: %b\n"
			 "shadow_stack: %b\n",
			 fault.present,
			 fault.write,
			 fault.user,
			 fault.reserved,
			 fault.instruction_fetch,
			 fault.protection_key,
			 fault.shadow_stack
	  );
	  panic("INT FAULT: Page Fault");
	  break;
	}
	case (15) :panic("INT FAULT:  Reserved");
	  break;
	case (16) :panic("INT FAULT:   ; Floating-Point Error (Math Fault)");
	  break;
	case (17) :panic("INT FAULT: Alignment Check");
	  break;
	case (18) :panic("INT FAULT: Machine Check");
	  break;
	case (19) :panic("INT FAULT: SIMD Floating-Point Exception");
	  break;
	case (20) :panic("INT FAULT:   ; Virtualization Exception");
	  break;
	case (21) :panic("INT FAULT: Control Protection Exception");
	  break;
	default:panic("UNKNOWN INTERRUPT FIRED")
	  break;
  }

}
