#include "stdint.h"
#define MAX_TASKS 4
#define STACK_SIZE 4096
typedef struct {
    uint32_t esp;   
    uint8_t  active;
} task_t;
static task_t tasks[MAX_TASKS];
static uint8_t current_task = 0;
static uint8_t task_count = 0;
static uint32_t stacks[MAX_TASKS][STACK_SIZE / 4] __attribute__((aligned(16)));
extern void shell_task(void);
extern void start_first_task(uint32_t esp);
extern void task1(void);
extern void task2(void);
static uint32_t* build_stack(int i, void (*entry)(void)) {
    uint32_t* s = &stacks[i][STACK_SIZE / 4];
    *(--s) = 0x00000202;          
    *(--s) = 0x00000008;          
    *(--s) = (uint32_t)entry;     
    *(--s) = 0; 
    *(--s) = 0; 
    *(--s) = 0; 
    *(--s) = 0;
    *(--s) = 0;
    *(--s) = 0; 
    *(--s) = 0;
    *(--s) = 0; 
    return s;
}
void scheduler_init(void) {
    tasks[0].esp = (uint32_t)build_stack(0, task1);
    tasks[0].active = 1;
    tasks[1].esp = (uint32_t)build_stack(1, task2);
    tasks[1].active = 1;
    tasks[2].esp = (uint32_t)build_stack(2, shell_task);
    tasks[2].active = 1;
    task_count = 3;
    current_task = 0;
}
void scheduler_start(void) {
    current_task = 0;
    __asm__ volatile("cli");
    start_first_task(tasks[0].esp);
}
static inline void vga_put(int x, int y, char c, uint8_t attr) {
    volatile uint16_t* vga = (uint16_t*)0xB8000;
    vga[y * 80 + x] = ((uint16_t)attr << 8) | (uint8_t)c;
}
static int task_index_from_esp(uint32_t esp) {
    for (int i = 0; i < task_count; i++) {
        uint32_t low  = (uint32_t)&stacks[i][0];
        uint32_t high = (uint32_t)&stacks[i][STACK_SIZE / 4];
        if (esp >= low && esp < high) return i;
    }
    return -1;
}
uint32_t schedule(uint32_t esp) {
    int running = task_index_from_esp(esp);
    if (running >= 0) {
        tasks[running].esp = esp;
        current_task = (uint8_t)running;
    }
    uint8_t next = current_task;
    do {
        next++;
        if (next >= task_count) next = 0;
    } while (!tasks[next].active && next != current_task);
    current_task = next;
    return tasks[current_task].esp;
}