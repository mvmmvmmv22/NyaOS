// kernel/kb_buf.h
#ifndef KB_BUF_H
#define KB_BUF_H

#include "stdint.h"

void kb_init(void);
int  kb_push(char c);     // 1 = ok, 0 = overflow
int  kb_pop(char* out);   // 1 = got char, 0 = empty

#endif
