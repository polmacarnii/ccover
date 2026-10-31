#ifndef DRAW_H
#define DRAW_H

#include "ccover.h"
#include "cover.h"
#include "disk.h"
#include "proc.h"
#include "terminal.h"

void clear_image(void);
void update_cover(const char *file, char *raw, char *cover, int *counter);

#endif
