#ifndef CCOVER_H
#define CCOVER_H

/* Include globals */
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <libgen.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* MACROS */
#define BUFFER 2048

/* Structs */
struct Song {
  char file[PATH_MAX];
  char title[BUFFER];
  char artist[BUFFER];
};

struct Progress {
  int position;
  int duration;
};

extern char cache_dir[PATH_MAX];

#endif
