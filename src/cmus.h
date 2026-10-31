#ifndef CMUS_H
#define CMUS_H

#include "ccover.h"
#include "proc.h"

int read_cmus(struct Song *song);
int song_changed(const struct Song *a, const struct Song *b);
void copy_song(struct Song *dst, const struct Song *src);

#endif
