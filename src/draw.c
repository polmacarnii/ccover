#include "draw.h"
#include <ftw.h>

/* Time to show the pictures on kitty*/
/* The image is drawed centered*/
static int center_row(int terminal, int image) {
  int row = (terminal - image) / 2;

  return row < 1 ? 1 : row;
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

static void show_with_term(const char *path, int image_size) {
  int term = terminal_rows();
  int cell_height = terminal_cell_height();
  int img = 12;

  if (cell_height > 0 && image_size > 0)
    img = (image_size + cell_height - 1) / cell_height;

  draw_image(path, center_row(term, img));
}

void update_cover(const char *file, char *raw, char *cover, int *counter) {
  char old[PATH_MAX];

  snprintf(old, sizeof(old), "%s", cover);

  (*counter)++;

  make_cache_names(raw, PATH_MAX, cover, PATH_MAX, *counter);

  int image_size = 0;
  int ok = extract_and_resize(file, raw, cover, &image_size) == 0;

  if (old[0])
    unlink(old);

  if (ok)
    show_with_term(cover, image_size);
}

void clear_image(void) {
  char *clear[] = {"kitty", "+kitten", "icat", "--clear", NULL};
  run_exec(clear, 1);
}
