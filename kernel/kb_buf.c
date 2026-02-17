// kernel/kb_buf.c
#include "kb_buf.h"

#define KBQ_SIZE 256

static volatile uint8_t head = 0;
static volatile uint8_t tail = 0;
static char buf[KBQ_SIZE];

void kb_init(void) {
    head = tail = 0;
}

int kb_push(char c) {
    uint8_t next = (uint8_t)(head + 1);
    if (next == tail) {
        return 0; // overflow
    }
    buf[head] = c;
    head = next;
    return 1;
}

int kb_pop(char* out) {
    if (tail == head) {
        return 0; // empty
    }
    *out = buf[tail];
    tail = (uint8_t)(tail + 1);
    return 1;
}
