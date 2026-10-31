#ifndef TERMINAL_H
#define TERMINAL_H

#include "proc.h"

int terminal_rows(void);
int terminal_cols(void);
int terminal_cell_height(void);
void clear_screen(void);

#endif
