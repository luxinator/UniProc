bits 64

; https://samwho.dev/blog/context-switching-on-x86/
; https://alex.dzyoba.com/blog/os-interrupts/
; https://github.com/s-matyukevich/raspberry-pi-os/blob/master/docs/lesson04/rpi-os.md
; https://en.wikibooks.org/wiki/X86_Assembly#Table_of_Contents

extern preempt_enable

global _switch_context
_switch_context:
; rdi contains pointer to the new proc context
; rax contains pointer to the old proc context
; load rdi into the stack pointers rsp, so when the interrupt returns a new stack will be popped

; Push working registers
    push rbx
    push rbp ; stack base
    push r11
    push r12
    push r13
    push r14
    push r15

    ; load stack pointer of new process
    mov rdi, rsp
    ;
    mov rsp, rsi ;<-- is this needed?

  ; restore the stack from  new task to the current rsp
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop rbx
    pop rbp

;; Return to the code on the stack
    ret
