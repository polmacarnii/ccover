#include "bar.h"

static int read_progress(struct Progress *p) {
  if (!p)
    return -1;

  p->position = 0;
  p->duration = 0;

  char buf[BUFFER];
  char *argv[] = {"cmus-remote", "-Q", NULL};

  if (capture_exec(argv, buf, sizeof(buf)) != 0)
    return -1;

  char *save;

  for (char *line = strtok_r(buf, "\r\n", &save); line;
       line = strtok_r(NULL, "\r\n", &save)) {
    if (strncmp(line, "position ", 9) == 0)
      p->position = atoi(line + 9);
    else if (strncmp(line, "duration ", 9) == 0)
      p->duration = atoi(line + 9);
  }

  return 0;
}

static void draw_progress_bar(const struct Progress *p) {
  putchar('[');

  if (p->duration <= 0) {

    for (int i = 0; i < BAR_WIDTH; i++)
      printf("─");

  } else {

    int cursor = (p->position * BAR_WIDTH) / p->duration;

    if (cursor >= BAR_WIDTH)
      cursor = BAR_WIDTH - 1;

    for (int i = 0; i < BAR_WIDTH; i++) {

      if (i < cursor)
        printf("█"); /* printf and not putchar because this simbol is UTF-8 and
                        uses multiple bytes, putchar draws only a byte */

      else if (i == cursor)
        printf("");

      else
        printf("─");
    }

    printf("]");
  }
}

static void format_progress_time(const struct Progress *p, char *buf,
                                 size_t size) {
  snprintf(buf, size, "%d:%02d / %d:%02d", p->position / 60, p->position % 60,
           p->duration / 60, p->duration % 60);
}

static int center(int total_width, int object_width) {
  int col = (total_width - object_width) / 2;
  return col < 0 ? 0 : col;
}

void print_info_and_bar(const char *title, const char *artist) {
  int rows = terminal_rows();
  int cols = terminal_cols();

  /* Some defaults */
  const char *t = (title && title[0]) ? title : "(Unknown)";
  const char *a = (artist && artist[0]) ? artist : "(Unknown)";

  struct Progress progress;
  char time[32];
  int base_row = rows - 4;

  printf("\033[%d;%dH\033[K %s", base_row, center(cols, strlen(t)), t);
  printf("\033[%d;%dH\033[K󰠃 %s", base_row + 1, center(cols, strlen(a)), a);

  if (read_progress(&progress) == 0) {
    printf("\033[%d;%dH\033[K", rows - 2, center(cols, BAR_WIDTH + 2));

    draw_progress_bar(&progress);

    format_progress_time(&progress, time, sizeof(time));

    printf("\033[%d;%dH\033[K%s", rows - 1, center(cols, strlen(time)), time);
  }

  fflush(stdout);
}
