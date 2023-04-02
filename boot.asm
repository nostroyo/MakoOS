ORG 0
BITS 16
_start:
    jmp short start
    nop

    times 33 db 0
start:
    jmp 0x7C0:step2

handle_int_zero:
    mov ah, 0eh
    mov al, 'A'
    mov bx, 0x00
    int 0x10
    iret


step2:
    cli ;clear int
    mov ax, 0x7c0
    mov ds, ax
    mov es, ax
    mov ax, 0x00
    mov ss, ax
    mov sp, 0x7C00
    sti ; Enables Int

    mov word[ss:0x00], handle_int_zero
    mov word[ss:0x02], 0x7c0

    int 0

    mov si, message
    call print
    jmp $

print:
    mov bx, 0
.loop:    
    lodsb
    cmp al, 0
    je .done
    call print_char
    jmp .loop
.done:    
    ret

print_char:
    mov ah, 0eh
    int 0x10
    ret

message: db 'Hello world!', 0

times 510 - ($ - $$) db 0
dw 0xAA55