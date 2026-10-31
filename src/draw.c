#include "draw.h"
#include <ftw.h>

/* Time to show the pictures on kitty*/
/* The image is drawed centered*/
static int center_row(int terminal, int image) {
  int row = (terminal - (image + image)) / 2;

  return row < 0 ? 0 : row;
}

static void draw_image(const char *path, int row) {
  char *clear[] = {"kitty", "+kitten", "icat", "--clear", NULL};

  run_exec(clear, 0);

  printf("\033[%d;1H", row);
  fflush(stdout);

  char *draw[] = {"kitty",      "+kitten",    "icat", "--transfer-mode=memory",
                  "--stdin=no", (char *)path, NULL};

  run_exec(draw, 0);
}

static void show_with_term(const char *path) {
  int term = terminal_rows();
  int img = 12;

  draw_image(path, center_row(term, img));
}

void update_cover(const char *file, char *raw, char *cover, int *counter) {
  (*counter)++;

  make_cache_names(raw, PATH_MAX, cover, PATH_MAX, *counter);

  if (extract_and_resize(file, raw, cover) == 0)
    show_with_term(cover);
}

void clear_image(void) {
  char *clear[] = {"kitty", "+kitten", "icat", "--clear", NULL};
  run_exec(clear, 1);
}
