#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SIZE 4

int main() {
  int fd[2];
  pipe(fd);
  pid_t child_pid = fork();
  switch (child_pid) {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);
    case 0:
      close(fd[1]);
      char buf[SIZE];
      read(fd[0], buf, SIZE);
      printf("%s\n", buf);
      exit(EXIT_SUCCESS);
      break;
    default:
      close(fd[0]);
      write(fd[1], "Hi!", SIZE);
      wait(NULL);
      break;
  }
  return EXIT_SUCCESS;
}