#define _XOPEN_SOURCE 700
#include "bar.h"
#include <wchar.h>

#define INFO_PREFIX_WIDTH 2

static int clamp_row(int row, int rows) {
  if (row < 1)
    return 1;
  if (row > rows)
    return rows;
  return row;
}

static size_t copy_to_width(const char *src, char *dst, size_t dstsz,
                            size_t max_width) {
  mbstate_t st;
  size_t width = 0;
  size_t out = 0;

  if (dstsz == 0)
    return 0;

  memset(&st, 0, sizeof(st));

  while (*src && out + 1 < dstsz) {
    wchar_t wc;
    size_t n = mbrtowc(&wc, src, MB_CUR_MAX, &st);
    size_t cw;

    if (n == (size_t)-1 || n == (size_t)-2) {
      memset(&st, 0, sizeof(st));
      if (width + 1 > max_width)
        break;
      dst[out++] = *src++;
      width++;
      continue;
    }

    if (n == 0)
      break;

    int w = wcwidth(wc);
    cw = w > 0 ? (size_t)w : 0;

    if (width + cw > max_width || out + n >= dstsz)
      break;

    memcpy(dst + out, src, n);
    out += n;
    src += n;
    width += cw;
  }

  dst[out] = '\0';
  return width;
}

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
  }

  printf("]");
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
  char tbuf[BUFFER];
  char abuf[BUFFER];
  size_t max_width =
      cols > INFO_PREFIX_WIDTH ? (size_t)(cols - INFO_PREFIX_WIDTH) : 0;
  size_t tw = copy_to_width(t, tbuf, sizeof(tbuf), max_width);
  size_t aw = copy_to_width(a, abuf, sizeof(abuf), max_width);
  int base_row = clamp_row(rows - 4, rows);

  printf("\033[%d;%dH\033[K %s", base_row,
         center(cols, (int)tw + INFO_PREFIX_WIDTH), tbuf);
  printf("\033[%d;%dH\033[K󰠃 %s", clamp_row(base_row + 1, rows),
         center(cols, (int)aw + INFO_PREFIX_WIDTH), abuf);

  if (read_progress(&progress) == 0) {
    printf("\033[%d;%dH\033[K", clamp_row(rows - 2, rows),
           center(cols, BAR_WIDTH + 2));

    draw_progress_bar(&progress);

    format_progress_time(&progress, time, sizeof(time));

    printf("\033[%d;%dH\033[K%s", clamp_row(rows - 1, rows),
           center(cols, (int)strlen(time)), time);
  }

  fflush(stdout);
}
