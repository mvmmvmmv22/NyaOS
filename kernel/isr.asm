BITS 32

GLOBAL irq0_stub
GLOBAL isr1
GLOBAL start_first_task

; exceptions 0..31
%assign i 0
%rep 32
GLOBAL isr%+i
%assign i i+1
%endrep

extern schedule
extern keyboard_handler
extern isr_exception_handler

extern isr_exception_handler_ex

; ========== common for NOERR exceptions ==========
; stack on entry (CPU): [EIP][CS][EFLAGS]
; we do: push int_no -> [int_no][EIP][CS][EFLAGS]
; after pusha: [regs][int_no][EIP][CS][EFLAGS]
isr_common_noerr:
    cli
    pusha

    mov eax, [esp + 32]   ; int_no
    mov ebx, [esp + 36]   ; EIP
    mov ecx, [esp + 40]   ; CS
    mov edx, [esp + 44]   ; EFLAGS

    push dword 0          ; err = 0
    push edx              ; eflags
    push ecx              ; cs
    push ebx              ; eip
    push eax              ; int_no
    call isr_exception_handler_ex
    add esp, 20

    popa
    iretd

; ========== common for ERR exceptions ==========
; CPU stack: [ERR][EIP][CS][EFLAGS]
; we push int_no -> [int_no][ERR][EIP][CS][EFLAGS]
; after pusha: [regs][int_no][ERR][EIP][CS][EFLAGS]
isr_common_err:
    cli
    pusha

    mov eax, [esp + 32]   ; int_no
    mov ebx, [esp + 36]   ; ERR
    mov ecx, [esp + 40]   ; EIP
    mov edx, [esp + 44]   ; CS
    mov esi, [esp + 48]   ; EFLAGS

    push ebx              ; err
    push esi              ; eflags
    push edx              ; cs
    push ecx              ; eip
    push eax              ; int_no
    call isr_exception_handler_ex
    add esp, 20

    popa
    add esp, 4            ; drop ERR before iretd
    iretd


%macro ISR_NOERR 1
isr%1:
    push dword %1
    jmp isr_common_noerr
%endmacro

%macro ISR_ERR 1
isr%1:
    push dword %1
    jmp isr_common_err
%endmacro

; --- exceptions ---
; NOERR: 0,1,2,3,4,5,6,7,9,15,16,18..31
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7

; ERR: 8,10,11,12,13,14,17
ISR_ERR   8

ISR_NOERR 9

ISR_ERR  10
ISR_ERR  11
ISR_ERR  12
ISR_ERR  13
ISR_ERR  14

ISR_NOERR 15
ISR_NOERR 16

ISR_ERR  17

ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31

start_first_task:
    mov eax, [esp+4]
    mov esp, eax

    ; ЖЁСТКО задаём нормальные флаги: IF=1, bit1=1
    mov dword [esp + 40], 0x00000202

    popa
    iretd



irq0_stub:
    cli
    pusha
    mov eax, esp
    push eax
    call schedule
    add esp, 4
    mov esp, eax
    mov dword [esp + 40], 0x00000202   ; <-- ВАЖНО: убираем TF

    mov al, 0x20
    out 0x20, al
    popa
    iretd



GLOBAL irq1_stub

; --- IRQ1 keyboard (vector 33) ---
irq1_stub:
    pusha
    call keyboard_handler
    popa
    mov al, 0x20
    out 0x20, al
    iretd
