#ifndef DISK_H
#define DISK_H

#include "ccover.h"
#define CACHE_DIR "/.cache/ccover" /* NO '~' here, pleasee */

int ensure_cache_dir(void);
void clear_dir(const char *dir);
void make_cache_names(char *out_raw, size_t rawsz, char *out_resized,
                      size_t resizedsz, int counter);
int file_exists(const char *path);
int copy_file(const char *src, const char *dst);

#endif
