bits 64

extern generic_handler
extern kernel_interrupt_handler
extern kernel_exception_handler

%macro interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
    push    0                     ; push 0 as error code
    push    %1                    ; push the interrupt number
    jmp     common_interrupt_handler    ; jump to the common handler
%endmacro

%macro error_code_handler 1
global error_code_handler_%1
error_code_handler_%1:
    push    %1                    ; push the interrupt number
    jmp     common_interrupt_handler    ; jump to the common handler
%endmacro

common_interrupt_handler:
    ; the common parts of the generic interrupt handler
    ; save the registers (pusha doest *almost* the same thing)
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
; ToDo: Swap pages
    ; call the C function
    cld ; not sure why this is here
    ; "Large" objects are passed through the stack, so the C code that is called fills the structs with the data from the stack
    call generic_handler
; ToDo: Swap pages
    ; restore the registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
    ; discard trapnum and errorcode
    add     rsp, 16
    ; return to the code that got interrupted (or to whatever is switched in after a context switch)
    iretq

; See intel software developer manual part 1 chapter 6.5
interrupt_handler  0       ; divde by zero
interrupt_handler  1       ; debug
interrupt_handler  2       ; NMI external interrupt
interrupt_handler  3       ; Breakpoint INT3 instruction
interrupt_handler  4       ; Overflow
interrupt_handler  5       ; Bound exceeds range
interrupt_handler  6       ; Invalid Opcode
interrupt_handler  7       ; Floating point exception, no Math Coprocessor
error_code_handler 8       ; Double Fault
interrupt_handler  9       ; CoProcessor Segment Overrun (reserved)
error_code_handler 10       ; invalid TSS (Task switch or TSS access)
error_code_handler 11       ; Segment not present
error_code_handler 12       ; Stack Segement Fault
error_code_handler 13       ; General Protection
error_code_handler 14       ; Page Fault
interrupt_handler  15       ;  Reserved
interrupt_handler  16       ; Floating-Point Error (Math Fault)
error_code_handler 17       ; Alignment Check
interrupt_handler  18       ; Machine Check
interrupt_handler  19       ; SIMD Floating-Point Exception
interrupt_handler  20       ; Virtualization Exception
interrupt_handler  21       ; Control Protection Exception
interrupt_handler  22       ;
interrupt_handler  23       ;
interrupt_handler  24       ;
interrupt_handler  25       ;
interrupt_handler  26       ;
interrupt_handler  27       ;
interrupt_handler  28       ;
interrupt_handler  29       ;
interrupt_handler  30       ;
interrupt_handler  31       ;

; IRQs
%assign i 32
%rep    16
    interrupt_handler i
%assign i i+1
%endrep

; user definded
%assign i 48
%rep    128
    interrupt_handler i
%assign i i+1
%endrep


global load_idt
load_idt:
    lidt [rdi]
    push 0x08
    lea rax, [rel .reload]
    push rax
    retfq
    .reload:
       mov ax, 0x10
       mov ds, ax
       mov fs, ax
       mov gs, ax
       mov ss, ax
       mov es, ax
       cli
       ret
