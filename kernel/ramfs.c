#include "stdint.h"
#define MAX_FILES 16
#define MAX_NAME 32
#define MAX_SIZE 256
uint8_t current_user = 0;   
typedef struct {
    char name[MAX_NAME];
    char data[MAX_SIZE];
    uint32_t size;
    uint8_t used;
    uint8_t owner;
} file_t;
static file_t files[MAX_FILES];
static int strcmp(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++; b++;
    }
    return *a == *b;
}
static void strcpy(char* dst, const char* src) {
    while (*src) {
        *dst++ = *src++;
    }
    *dst = 0;
}
void ramfs_init() {
    for (int i = 0; i < MAX_FILES; i++)
        files[i].used = 0;
}
int ramfs_create(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!files[i].used) {
            strcpy(files[i].name, name);
            files[i].size = 0;
            files[i].used = 1;
            files[i].owner = current_user;
            return 1;
        }
    }
    return 0;
}
file_t* ramfs_find(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, name))
            return &files[i];
    }
    return 0;
}
void ramfs_write(const char* name, const char* text) {
    file_t* f = ramfs_find(name);
    if (!f) return;
    int i = 0;
    while (text[i] && i < MAX_SIZE - 1) {
        f->data[i] = text[i];
        i++;
    }
    f->data[i] = 0;
    f->size = i;
}
void ramfs_list(void (*print)(const char*)) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used) {
            print(files[i].name);
            print("\n");
        }
    }
}
int ramfs_delete(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, name)) {
            if (files[i].owner != current_user && current_user != 0)
                return -1;   
            files[i].used = 0;
            files[i].size = 0;
            files[i].name[0] = 0;
            return 1;      
        }
    }
    return 0;  
}
void ramfs_cat(const char* name, void (*print)(const char*)) {
    file_t* f = ramfs_find(name);
    if (!f) return;
    print(f->data);
    print("\n");
}