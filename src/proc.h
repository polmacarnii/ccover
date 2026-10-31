#ifndef PROC_H
#define PROC_H

#include "ccover.h"

void handle_sig(int sig);
int capture_exec(char *const argv[], char *buf, size_t size);
int run_exec(char *const argv[], int quiet);

#endif
