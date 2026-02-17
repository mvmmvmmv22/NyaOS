BITS 32
GLOBAL protected_start
EXTERN kernel_main
protected_start:
    mov eax, [esp]        
    mov ebx, [esp+4]      
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9FC00
    push ebx             
    push eax              
    call kernel_main
    jmp $
