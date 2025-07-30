#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define WIDTH_NAME 10
#define WIDTH_PID 7

void err_check(pid_t pid) {
  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
}

void print_pid(char* pr_name){
  printf("%-*s pid:%-*d %-*s ppid:%-*d\n", WIDTH_NAME, pr_name, WIDTH_PID,
        getpid(), WIDTH_NAME, pr_name, WIDTH_PID, getppid());
}

int main() {
  pid_t pid1, pid2;

  pid1 = fork();
  err_check(pid1);

  if (pid1 > 0) pid2 = fork();
  err_check(pid2);

  if (pid1 == 0) {
    pid_t pid3, pid4;

    pid3 = fork();
    err_check(pid3);

    if (pid3 > 0) pid4 = fork();
    err_check(pid4);

    if (pid3 == 0) {
      print_pid("pr1_child1");
      exit(EXIT_SUCCESS);
    } else if (pid4 == 0) {
      print_pid("pr1_child2");
      exit(EXIT_SUCCESS);
    } else {
      print_pid("pr1");
      wait(NULL);
      exit(EXIT_SUCCESS);
    }
  } else if (pid2 == 0) {
    pid_t pid5;

    pid5 = fork();
    err_check(pid5);

    if (pid5 == 0) {
      print_pid("pr2_child");
      exit(EXIT_SUCCESS);
    } else {
      print_pid("pr2");
      wait(NULL);
      exit(EXIT_SUCCESS);
    }

  } else {
    print_pid("parent");
    wait(NULL);
  }
  return EXIT_SUCCESS;
}