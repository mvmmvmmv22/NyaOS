[org 0x7C00]
[bits 16]
KERNEL_ADDR equ 0x1000
KERNEL_SECTORS equ 20
start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti
load_kernel:
    mov bx, KERNEL_ADDR   
    mov es, ax            
    mov ah, 0x02          
    mov al, KERNEL_SECTORS
    mov ch, 0x00          
    mov cl, 0x02          
    mov dh, 0x00                   
    int 0x13
    jc disk_error
mov di, 0x500
xor ebx, ebx
xor bp, bp        
e820_loop:
    xor ax, ax
    mov es, ax
    mov eax, 0xE820
    mov ecx, 24
    mov edx, 0x534D4150
    int 0x15
    jc e820_done
    cmp eax, 0x534D4150
    jne e820_done
    add di, 24
    inc bp         
    cmp ebx, 0
    jne e820_loop
e820_done:
enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:protected_mode
[bits 32]
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000
    mov eax, 0x500    
    movzx ebx, bp      
    push ebx           
    push eax           
    jmp KERNEL_ADDR
hang:
    hlt
    jmp hang
disk_error:
    cli
    hlt
    jmp $
gdt_start:
    dq 0x0000000000000000
    dq 0x00CF9A000000FFFF   
    dq 0x00CF92000000FFFF   
gdt_end:
gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start
times 510-($-$$) db 0
dw 0xAA55