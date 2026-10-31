#include "proc.h"

extern volatile sig_atomic_t running;

void handle_sig(int sig) {
  (void)sig;
  running = 0;
}

int capture_exec(char *const argv[], char *buf, size_t size) {
  if (!argv || !buf || size == 0)
    return -1;

  int pipefd[2];
  if (pipe(pipefd) == -1) {
    buf[0] = '\0';
    return -1;
  }

  pid_t pid = fork();
  if (pid == -1) {
    close(pipefd[0]);
    close(pipefd[1]);
    buf[0] = '\0';
    return -1;
  }

  if (pid == 0) {
    close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);

    int fd = open("/dev/null", O_WRONLY);
    if (fd != -1) {
      dup2(fd, STDERR_FILENO);
      if (fd > STDERR_FILENO)
        close(fd);
    }

    execvp(argv[0], argv);
    _exit(127);
  }

  close(pipefd[1]);

  size_t total = 0;
  char discard[512];

  for (;;) {
    ssize_t n;
    char *dst = total + 1 < size ? buf + total : discard;
    size_t dstsz = total + 1 < size ? size - 1 - total : sizeof(discard);

    n = read(pipefd[0], dst, dstsz);
    if (n == 0)
      break;
    if (n < 0) {
      if (errno == EINTR)
        continue;
      break;
    }

    if (dst == buf + total)
      total += (size_t)n;
  }

  close(pipefd[0]);

  int status = 0;
  if (waitpid(pid, &status, 0) == -1) {
    buf[total] = '\0';
    return -1;
  }

  buf[total] = '\0';

  if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
    return -1;

  return 0;
}

int run_exec(char *const argv[], int quiet) {
  pid_t pid = fork();
  if (pid == -1)
    return -1;

  if (pid == 0) {
    if (quiet) {
      int fd = open("/dev/null", O_WRONLY);
      if (fd != -1) {
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if (fd > STDERR_FILENO)
          close(fd);
      }
    }

    execvp(argv[0], argv);
    _exit(127);
  }

  int status = 0;

  if (waitpid(pid, &status, 0) == -1)
    return -1;

  return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}
