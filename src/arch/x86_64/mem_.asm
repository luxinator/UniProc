bits 64

global load_page_table
load_page_table:
    ret

global report_cr3
report_cr3:
    mov rax, cr3
    ret


global flush_tlb
flush_tlb:
    mov rax, cr3
    mov cr3, rax
    ret

global read_cr2
read_cr2:
    mov rax, cr2
    ret