#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "stdint.h"
#define KB_BUF_SIZE 256
void keyboard_handler(void);
void set_cursor_pos(int pos);
#endif
