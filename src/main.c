/* Includes */
#define _XOPEN_SOURCE 700 /* For POSIX functions to work */
#include "bar.h"
#include "cmus.h"
#include "disk.h"
#include "draw.h"
#include "proc.h"
#include <signal.h>

/* Variables */
volatile sig_atomic_t running = 1;
volatile sig_atomic_t resized = 0;

void handle_resize(int sig) {
  (void)sig;
  resized = 1;
}

static int setup(void) {
  struct sigaction sa = {0};

  /* SIGINT and SIGTERM */
  sa.sa_handler = handle_sig;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  if (sigaction(SIGINT, &sa, NULL) == -1)
    return -1;

  if (sigaction(SIGTERM, &sa, NULL) == -1)
    return -1;

  /* SIGWINCH */
  sa.sa_handler = handle_resize;

  if (sigaction(SIGWINCH, &sa, NULL) == -1)
    return -1;

  if (ensure_cache_dir() != 0)
    return -1;

  clear_screen();
  fflush(stdout);

  return 0;
}

static void kitty_clear(void) {
  char *argv[] = {"kitty", "+kitten", "icat", "--clear", NULL};
  run_exec(argv, 1);
}

static void cleanup(void) {
  kitty_clear();
  clear_dir(cache_dir);
  rmdir(cache_dir);
  printf("\033[2J\033[H");
}

static void wait_update(void) {
  struct timespec ts = {.tv_sec = 0, .tv_nsec = 100000000};

  for (int i = 0; i < 10 && running; i++)
    nanosleep(&ts, NULL);
}

static void no_song(void) {
  clear_screen();
  clear_image();
  puts("CMUS is not running (or there is not any song).");
  fflush(stdout);
}

/* main */
int main(void) {
  struct Song song = {0};
  struct Song prev = {0};

  char raw[PATH_MAX];
  char cover[PATH_MAX];

  if (setup() != 0)
    return 1;

  int counter = 0;
  while (running) {
    read_cmus(&song);
    if (!song.file[0]) {
      no_song();
      wait_update();
      continue;
    }

    if (song_changed(&song, &prev)) {
      clear_screen();
      kitty_clear();
      update_cover(song.file, raw, cover, &counter);
      copy_song(&prev, &song);
    }

    if (resized == 1) {
      resized = 0;
      kitty_clear();
      clear_screen();
      update_cover(song.file, raw, cover, &counter);
    }
    print_info_and_bar(song.title, song.artist);
    wait_update();
  }

  cleanup();
  return 0;
}
