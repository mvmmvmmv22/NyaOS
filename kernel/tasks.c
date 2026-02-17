#include "stdint.h"
static inline void vga_put(int x, int y, char c, uint8_t attr) {
    volatile uint16_t* vga = (uint16_t*)0xB8000;
    vga[y * 80 + x] = ((uint16_t)attr << 8) | (uint8_t)c;
}
extern void putchar(char c);
void task1(void) {
    while (1) {
        vga_put(1, 0, 'A', 0x0F); 
        for (volatile int i = 0; i < 2000000; i++);
    }
}
extern void shell_add_char(char c);
extern int kb_pop(char* out);
void shell_task(void) {
    while (1) {
        char c;
        if (kb_pop(&c)) {
            putchar(c);
            shell_add_char(c);
        } else {
            __asm__ volatile("hlt");
        }
    }
}
void task2(void) {
    while (1) {
        vga_put(1, 0, 'B', 0x0F);  
        for (volatile int i = 0; i < 2000000; i++);
    }
}