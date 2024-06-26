bits 64
global load_gdt
load_gdt:
    lgdt [rdi]
    push 0x08
    lea rax, [rel .flush]
    push rax
    retfq

    .flush:
        mov ax, 0x10
        mov ds, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        mov es, ax
        ret

global tss_update
tss_update:
    ; 0x28 -> sixth entry in the GDT
    mov ax, 0x28
    ltr ax
    ret