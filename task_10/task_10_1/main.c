#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  int status;
  pid_t child_pid = fork();
  switch (child_pid) {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);
    case 0:
      printf("child_pid: %d  child_ppid: %d\n", getpid(), getppid());
      exit(5);
      break;
    default:
      printf("parent_pid: %d  parent_ppid: %d\n", getpid(), getppid());
      wait(&status);
      printf("status: %d\n", WEXITSTATUS(status));
      break;
  }
  return EXIT_SUCCESS;
}