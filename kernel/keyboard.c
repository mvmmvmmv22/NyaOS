extern void shell_add_char(char c);
#include "stdint.h"
#include "kb_buf.h"
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000
static uint8_t shift = 0;
static int cursor_pos = 0;
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
void update_cursor() {
    uint16_t pos = cursor_pos;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}
void enable_cursor() {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0);
    outb(0x3D4, 0x0B);
    outb(0x3D5, 15);
}
void scroll() {
    char* vga = (char*)VGA_MEMORY;
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            int from = (y * VGA_WIDTH + x) * 2;
            int to   = ((y - 1) * VGA_WIDTH + x) * 2;
            vga[to]     = vga[from];
            vga[to + 1] = vga[from + 1];
        }
    }
    for (int x = 0; x < VGA_WIDTH; x++) {
        int pos = ((VGA_HEIGHT - 1) * VGA_WIDTH + x) * 2;
        vga[pos]     = ' ';
        vga[pos + 1] = 0x0F;
    }
    cursor_pos -= VGA_WIDTH;
}
static const char scancode_table[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,
    '\\','z','x','c','v','b','n','m',',','.','/',
    0,
    '*',
    0,
    ' ',
};
static const char scancode_table_shift[128] = {
    0,  27, '!','@','#','$','%','^','&','*','(',')','_','+','\b',
    '\t',
    'Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,
    'A','S','D','F','G','H','J','K','L',':','"','~',
    0,
    '|','Z','X','C','V','B','N','M','<','>','?',
    0,
    '*',
    0,
    ' ',
};
void putchar(char c) {
    char* vga = (char*)VGA_MEMORY;
    if (c == '\n') {
        cursor_pos += VGA_WIDTH - (cursor_pos % VGA_WIDTH);
    }
    else if (c == '\b') {
        if (cursor_pos > 160 && (cursor_pos % 80) > 2) {
            cursor_pos--;
            vga[cursor_pos * 2] = ' ';
            vga[cursor_pos * 2 + 1] = 0x0F;
        }
    }
    else {
        vga[cursor_pos * 2] = c;
        vga[cursor_pos * 2 + 1] = 0x0F;
        cursor_pos++;
    }
    if (cursor_pos >= VGA_WIDTH * VGA_HEIGHT) {
        scroll();
    }
    update_cursor();
}
void keyboard_handler() {
    uint8_t scancode = inb(0x60);
    if (scancode == 0x2A || scancode == 0x36) { shift = 1; return; }
    if (scancode == 0xAA || scancode == 0xB6) { shift = 0; return; }
    if (scancode & 0x80) return; 
    char c = shift ? scancode_table_shift[scancode] : scancode_table[scancode];
    if (c) {
        kb_push(c);
    }
}
void set_cursor_pos(int pos) {
    cursor_pos = pos;
    update_cursor();
}
void print_hex8(uint8_t v) {
    const char* h = "0123456789ABCDEF";
    putchar(h[(v >> 4) & 0xF]);
    putchar(h[v & 0xF]);
}
void print_str(const char* s) {
    while (*s) putchar(*s++);
}