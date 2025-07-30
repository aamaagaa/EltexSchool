#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARGS 64

void err_check(pid_t pid) {
  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
}

char** parse_input(char* input) {
  char** args = malloc(MAX_ARGS * sizeof(char*));
  char* token = NULL;
  int n = 0;
  token = strtok(input, " ");

  while (token != NULL && n < MAX_ARGS - 1) {
    args[n] = token;
    token = strtok(NULL, " ");
    n++;
  }
  args[n] = NULL;
  return args;
}

int main() {
  pid_t pid;
  char* input = NULL;
  size_t len = 0;
  ssize_t nread;
  printf("mini-bash $ ");
  while ((nread = getline(&input, &len, stdin)) != -1) {
    input[strcspn(input, "\n")] = '\0';
    if (strncmp("exit", input, nread - 1) == 0) {
      free(input);
      return EXIT_SUCCESS;
    }

    pid = fork();
    err_check(pid);

    if (pid == 0) {
      char** args = parse_input(input);
      if (args[0] == NULL) {
        free(args);
        continue;
      }
      execvp(args[0], args);
      free(args);
      exit(EXIT_FAILURE);
    } else {
      wait(NULL);
    }
    printf("mini-bash $ ");
  }

  free(input);
  return EXIT_SUCCESS;
}