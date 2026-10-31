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
    execvp(argv[0], argv);
    _exit(127);
  }

  close(pipefd[1]);

  size_t total = 0;
  ssize_t n;

  while (total + 1 < size &&
         (n = read(pipefd[0], buf + total, size - 1 - total)) > 0)
    total += (size_t)n;

  close(pipefd[0]);

  int status = 0;
  waitpid(pid, &status, 0);

  buf[total] = '\0';
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
