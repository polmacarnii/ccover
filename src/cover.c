#include "cover.h"

static int extract_cover(const char *audio, const char *output) {
  char *argv[] = {"ffmpeg", "-y",    "-i",           (char *)audio,
                  "-map",   "0:v:0", "-frames:v",    "1",
                  "-c",     "copy",  (char *)output, NULL};

  if (run_exec(argv, 1) != 0)
    return -1;

  return file_exists(output) ? 0 : -1;
}

static int find_cover(const char *audio, const char *output) {
  static const char *names[] = {"cover.jpg",  "cover.png", "cover.jpeg",
                                "folder.jpg", "front.jpg", "art.jpg",
                                NULL};

  char dir[PATH_MAX];
  char path[PATH_MAX];

  strncpy(dir, audio, sizeof(dir) - 1);
  dir[sizeof(dir) - 1] = '\0';

  char *base = dirname(dir);

  for (int i = 0; names[i]; i++) {
    snprintf(path, sizeof(path), "%s/%s", base, names[i]);
    if (!file_exists(path))
      continue;

    if (copy_file(path, output) == 0 && file_exists(output))
      return 0;
  }

  return -1;
}

static int resize_cover(const char *input, const char *output,
                        int *image_size) {
  char vf[256];
  int img_size;
  int rows = terminal_rows();

  if (rows <= 24)
    img_size = 256;
  else if (rows < 55)
    img_size = 365;
  else if (rows < 80)
    img_size = 512;
  else if (rows < 90)
    img_size = 720;
  else
    img_size = 850;

  if (image_size)
    *image_size = img_size;

  snprintf(vf, sizeof(vf),
           "scale=%d:%d:force_original_aspect_ratio=decrease,"
           "pad=%d:%d:(ow-iw)/2:(oh-ih)/2",
           img_size, img_size, img_size, img_size);

  char *argv1[] = {"ffmpeg", "-y",   "-i",    (char *)input,  "-vf",
                   vf,       "-c:v", "mjpeg", (char *)output, NULL};

  if (run_exec(argv1, 1) == 0 && file_exists(output))
    return 0;

  char *argv2[] = {"ffmpeg", "-y", "-i",           (char *)input,
                   "-vf",    vf,   (char *)output, NULL};

  if (run_exec(argv2, 1) != 0)
    return -1;

  return 0;
}

int extract_and_resize(const char *audio, const char *raw, const char *cover,
                       int *image_size) {
  if (!audio || !raw || !cover)
    return -1;

  if (image_size)
    *image_size = 0;

  if (extract_cover(audio, raw) != 0)
    if (find_cover(audio, raw) != 0) {
      unlink(raw);
      return -1;
    }

  if (resize_cover(raw, cover, image_size) != 0) {
    unlink(raw);
    unlink(cover);
    return -1;
  }

  unlink(raw);

  return 0;
}
