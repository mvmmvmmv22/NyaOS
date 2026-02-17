#include "stdint.h"
#include "idt.h"
extern void set_cursor_pos(int pos);
extern void clear_screen();
extern void putchar(char c);
extern uint32_t get_tick();
extern uint64_t total_memory;
extern uint32_t kernel_end;
void ramfs_list(void (*print)(const char*));
void ramfs_cat(const char* name, void (*print)(const char*));
int  ramfs_create(const char* name);
void ramfs_write(const char* name, const char* text);
int ramfs_delete(const char* name);
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}
static char input_buffer[128];
static int input_pos = 0;
int strcmp(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++; b++;
    }
    return (*a == 0 && *b == 0);
}
static int starts_with(const char* str, const char* prefix) {
    while (*prefix) {
        if (*str++ != *prefix++) return 0;
    }
    return 1;
}
void print(const char* str) {
    while (*str) putchar(*str++);
}
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_ptr_t;
static int check_protected_mode() {
    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    return (cr0 & 1);
}
static int check_gdt() {
    gdt_ptr_t gdtr;
    __asm__ volatile ("sgdt %0" : "=m"(gdtr));
    return (gdtr.limit != 0);
}
static int check_idt() {
    idt_ptr_t idtr;
    __asm__ volatile ("sidt %0" : "=m"(idtr));
    return (idtr.limit != 0);
}
void execute_command() {
    if (strcmp(input_buffer, "help")) {
        print("\nCommands: help clear mem reboot shutdown sysinfo ls touch cat write\n");
    }
    else if (strcmp(input_buffer, "clear")) {
        clear_screen();
        set_cursor_pos(160);
    }
    else if (strcmp(input_buffer, "reboot")) {
        print("\nRebooting...\n");
        __asm__ volatile ("cli");
        outb(0x64, 0xFE);
    }
    else if (strcmp(input_buffer, "shutdown")) {
        print("\nPowering off...\n");
        outw(0x604, 0x2000);
        __asm__ volatile ("hlt");
    }
    else if (strcmp(input_buffer, "mem")) {
        uint64_t mem = total_memory;
        uint32_t mem_mb = (uint32_t)(mem / (1024 * 1024));
        print("\nTotal RAM: ");
        char buf[16];
        int i = 0;
        if (mem_mb == 0) {
            putchar('0');
        } else {
            while (mem_mb > 0) {
                buf[i++] = '0' + (mem_mb % 10);
                mem_mb /= 10;
            }
            while (i > 0) {
                putchar(buf[--i]);
            }
        }
        print(" MB\n");
    }
    else if (strcmp(input_buffer, "sysinfo")) {
        print("\n=== System Info ===\n");
        if (check_protected_mode())
            print("Mode: Protected 32-bit\n");
        else
            print("Mode: Real\n");
        if (check_gdt())
            print("GDT: Loaded\n");
        else
            print("GDT: Not loaded\n");
        if (check_idt())
            print("IDT: Loaded\n");
        else
            print("IDT: Not loaded\n");
        uint32_t seconds = get_tick() / 100;
        print("Uptime: ");
        char buf[16];
        int i = 0;
        if (seconds == 0) {
            putchar('0');
        } else {
            while (seconds > 0) {
                buf[i++] = '0' + (seconds % 10);
                seconds /= 10;
            }
            while (i > 0)
                putchar(buf[--i]);
        }
        print(" sec\n");
    }
    else if (strcmp(input_buffer, "ls")) {
        print("\n");
        ramfs_list(print);
    }
    else if (starts_with(input_buffer, "touch ")) {
        char* name = input_buffer + 6;
        if (ramfs_create(name))
            print("\nFile created\n");
        else
            print("\nFailed\n");
    }
    else if (starts_with(input_buffer, "cat ")) {
        char* name = input_buffer + 4;
        print("\n");
        ramfs_cat(name, print);
    }
    else if (starts_with(input_buffer, "write ")) {
        char* args = input_buffer + 6;
        char* space = args;
        while (*space && *space != ' ') space++;
        if (*space) {
            *space = 0;
            char* name = args;
            char* text = space + 1;
            ramfs_write(name, text);
            print("\nWritten\n");
        }
    }
    else if (starts_with(input_buffer, "rm ")) {
        char* name = input_buffer + 3;
        int result = ramfs_delete(name);
        if (result == 1)
            print("\nDeleted\n");
        else if (result == -1)
            print("\nPermission denied\n");
        else
            print("\nFile not found\n");
    }
    else if (input_pos > 0) {
        print("\nUnknown command\n");
    }
    input_pos = 0;
    input_buffer[0] = 0;
    print("> ");
}
void shell_add_char(char c) {
    if (c == '\n') {
        input_buffer[input_pos] = 0;
        execute_command();
    }
    else if (c == '\b') {
        if (input_pos > 0) {
            input_pos--;
            input_buffer[input_pos] = 0;
        }
    }
    else {
        if (input_pos < 127) {
            input_buffer[input_pos++] = c;
        }
    }
}
void shell_init() {
    print("> ");
}
