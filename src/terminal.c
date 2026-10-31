#include "terminal.h"

int terminal_rows(void) {
  struct winsize ws;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_row > 0)
    return ws.ws_row;

  return 30;
}

int terminal_cols(void) {
  struct winsize ws;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0)
    return ws.ws_col;

  return 80;
}

void clear_screen(void) {
  printf("\033[3J\033[2J\033[H");
  fflush(stdout);
}
