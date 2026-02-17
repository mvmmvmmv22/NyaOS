#include "stdint.h"
extern void print_str(const char* s);
extern void print_hex8(uint8_t v);
extern void putchar(char c);
static void print_hex32(uint32_t v) {
    const char* h = "0123456789ABCDEF";
    for (int i = 7; i >= 0; i--) {
        putchar(h[(v >> (i * 4)) & 0xF]);
    }
}
static void print_hex16(uint16_t v) {
    const char* h = "0123456789ABCDEF";
    putchar(h[(v >> 12) & 0xF]);
    putchar(h[(v >>  8) & 0xF]);
    putchar(h[(v >>  4) & 0xF]);
    putchar(h[(v >>  0) & 0xF]);
}
void isr_exception_handler_ex(uint32_t int_no, uint32_t eip, uint32_t cs, uint32_t eflags, uint32_t err) {
    print_str("\nEXC ");
    print_hex8((uint8_t)int_no);
    print_str(" EIP=");
    print_hex32(eip);
    print_str(" CS=");
    print_hex16((uint16_t)cs);
    print_str(" FL=");
    print_hex32(eflags);
    print_str(" ERR=");
    print_hex32(err);
    print_str("\nHALT\n");
    for (;;) {
        __asm__ volatile("cli; hlt");
    }
}
