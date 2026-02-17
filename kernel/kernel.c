extern void idt_init();
extern void pit_init();
extern void shell_init();
extern void enable_cursor(); 
extern void scheduler_init();
extern void scheduler_start();
extern void ramfs_init();
extern void kb_init();
extern void set_cursor_pos(int pos);
#include "stdint.h"
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
void clear_screen() {
    char* vga = (char*)0xB8000;
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga[i * 2] = ' ';      
        vga[i * 2 + 1] = 0x07; 
    }
}
typedef struct {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t acpi;
} __attribute__((packed)) e820_entry_t;
uint64_t total_memory = 0;
void parse_memory_map(uint32_t mmap_addr, uint32_t entries) {

    e820_entry_t* entry = (e820_entry_t*)mmap_addr;

    for (uint32_t i = 0; i < entries; i++){

        if (entry[i].length == 0)
            break;

        if (entry[i].type == 1) { 
            total_memory += entry[i].length;
        }
    }
}
void kernel_main(uint32_t mmap_addr, uint32_t mmap_entries){
    clear_screen();
    enable_cursor();
    ramfs_init();
    parse_memory_map(mmap_addr, mmap_entries);
    char* vga = (char*)0xB8000;
    const char* msg = "MyaKRNL Booted!";
    for (int i = 0; msg[i]; i++) {
        vga[i*2] = msg[i];
        vga[i*2+1] = 0x0F;
    }
    idt_init();
    pit_init();
    set_cursor_pos(80);
    shell_init();
    kb_init();

    scheduler_init();
    __asm__ volatile("sti");
    scheduler_start();  
    while (1) {
        __asm__("hlt");
    }
}