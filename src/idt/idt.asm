section .asm

extern int21h_handler
global idt_load
global int21h
extern no_interrupt_handler
global no_interrupt
global enable_int
global desible_int

enable_int:
    sti
    ret
disable_int:
    cli
    ret
idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp+8]
    lidt [ebx]
    pop ebp
    ret

int21h:
    cli
    pushad
    
    call int21h_handler
    
    popad
    sti
    
    iret

no_interrupt:
    cli
    pushad
    
    call no_interrupt_handler
    
    popad
    sti
    
    iret