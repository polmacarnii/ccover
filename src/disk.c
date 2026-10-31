#define _XOPEN_SOURCE 700
#include "disk.h"

char cache_dir[PATH_MAX] = {0};

static int mkpath(char *path) {
  char *p;

  for (p = path + 1; *p; p++) {
    if (*p != '/')
      continue;

    *p = '\0';

    if (mkdir(path, 0700) == -1 && errno != EEXIST)
      return -1;

    *p = '/';
  }

  return mkdir(path, 0700) == -1 && errno != EEXIST ? -1 : 0;
}

int ensure_cache_dir(void) { /* 0 -> exists, 1 -> does not exist */
  struct stat st;
  const char *home;

  home = getenv("HOME");
  if (!home)
    return -1;

  snprintf(cache_dir, sizeof(cache_dir), "%s%s", home,
           CACHE_DIR); /* Build the full path */

  if (stat(cache_dir, &st) == 0)
    return S_ISDIR(st.st_mode)
               ? 0
               : -1; /* if is a file and NOT a dir, its an error */

  return mkpath(cache_dir);
}

static int rm(const char *path, const struct stat *st, int type,
              struct FTW *ftwbuf) {
  (void)st;
  (void)ftwbuf;

  return type == FTW_DP ? rmdir(path) : unlink(path);
}

void clear_dir(const char *dir) {
  nftw(dir, rm, 64, FTW_DEPTH | FTW_PHYS); /* Oh yeah, POSIX */
}

void make_cache_names(char *out_raw, size_t rawsz, char *out_resized,
                      size_t resizedsz, int counter) {
  /* This fucntion is kinda fun, because based on the PID is how we make the
   * name!, so its kinda "unique", and also we have a counter yay */
  pid_t pid = getpid();
  snprintf(out_raw, rawsz, "%s/cover-%d-%d-raw.jpg", cache_dir, (int)pid,
           counter);

  snprintf(out_resized, resizedsz, "%s/cover-%d-%d.jpg", cache_dir, (int)pid,
           counter);
}

int file_exists(const char *path) {
  struct stat st;

  if (!path || stat(path, &st) == -1)
    return 0;

  return S_ISREG(st.st_mode);
}

int copy_file(const char *src, const char *dst) {
  int in = open(src, O_RDONLY);
  if (in == -1)
    return -1;

  struct stat st;
  if (fstat(in, &st) == -1) {
    close(in);
    return -1;
  }

  int out = open(dst, O_WRONLY | O_CREAT | O_TRUNC, st.st_mode & 0777);
  if (out == -1) {
    close(in);
    return -1;
  }

  char buf[8192];
  ssize_t n;
  int err = 0;

  while ((n = read(in, buf, sizeof(buf))) > 0) {
    ssize_t w = 0;
    while (w < n) {
      ssize_t k = write(out, buf + w, (size_t)(n - w));
      if (k == -1) {
        err = -1;
        goto done;
      }
      w += k;
    }
  }

  if (n == -1)
    err = -1;

done:
  close(in);
  if (close(out) == -1)
    err = -1;
  return err;
}
