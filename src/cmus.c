#include "cmus.h"

int read_cmus(struct Song *song) {
  if (!song)
    return -1;

  song->file[0] = '\0';
  song->title[0] = '\0';
  song->artist[0] = '\0';

  char buf[BUFFER];
  char *argv[] = {"cmus-remote", "-Q", NULL};

  if (capture_exec(argv, buf, sizeof(buf)) != 0)
    return -1;

  char *save;

  for (char *line = strtok_r(buf, "\r\n", &save); line;
       line = strtok_r(NULL, "\r\n", &save)) {
    if (strncmp(line, "file ", 5) == 0)
      snprintf(song->file, sizeof(song->file), "%s", line + 5);
    else if (strncmp(line, "tag title ", 10) == 0)
      snprintf(song->title, sizeof(song->title), "%s", line + 10);
    else if (strncmp(line, "tag artist ", 11) == 0)
      snprintf(song->artist, sizeof(song->artist), "%s", line + 11);
  }

  return 0;
}

int song_changed(const struct Song *a, const struct Song *b) {
  return strcmp(a->file, b->file) != 0;
}

void copy_song(struct Song *dst, const struct Song *src) { *dst = *src; }
