#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARGS 64

typedef struct {
  char** pre_pipe_args;
  char** post_pipe_args;
  int is_pipe;
} ParsedCommand;

void err_check(pid_t pid) {
  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
}

ParsedCommand parse_input(char* input) {
  ParsedCommand parsed_command = {0};
  parsed_command.pre_pipe_args = malloc(MAX_ARGS * sizeof(char*));
  parsed_command.post_pipe_args = malloc(MAX_ARGS * sizeof(char*));
  parsed_command.is_pipe = 0;

  char* token = strtok(input, " ");
  int pre_count = 0;
  int post_count = 0;

  while (token != NULL && (pre_count + post_count) < MAX_ARGS - 2) {
    if (!parsed_command.is_pipe && strcmp(token, "|") == 0) {
      parsed_command.is_pipe = 1;
      token = strtok(NULL, " ");
      continue;
    }

    if (parsed_command.is_pipe) {
      parsed_command.post_pipe_args[post_count++] = token;
    } else {
      parsed_command.pre_pipe_args[pre_count++] = token;
    }

    token = strtok(NULL, " ");
  }

  parsed_command.pre_pipe_args[pre_count] = NULL;
  parsed_command.post_pipe_args[post_count] = NULL;

  return parsed_command;
}

void free_parsed_command(ParsedCommand* cmd) {
  free(cmd->pre_pipe_args);
  if (cmd->is_pipe) {
    free(cmd->post_pipe_args);
  }
}

int main() {
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
    ParsedCommand parsed_command = parse_input(input);

    if (parsed_command.is_pipe) {
      int fd[2];
      pipe(fd);

      pid_t pid1 = fork();
      err_check(pid1);

      if (pid1 == 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);

        execvp(parsed_command.pre_pipe_args[0], parsed_command.pre_pipe_args);
        exit(EXIT_FAILURE);
      }

      pid_t pid2 = fork();
      err_check(pid2);

      if (pid2 == 0) {
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);

        execvp(parsed_command.post_pipe_args[0], parsed_command.post_pipe_args);
        exit(EXIT_FAILURE);
      }

      close(fd[0]);
      close(fd[1]);
      waitpid(pid1, NULL, 0);
      waitpid(pid2, NULL, 0);
    } else {
      pid_t pid = fork();
      err_check(pid);

      if (pid == 0) {
        execvp(parsed_command.pre_pipe_args[0], parsed_command.pre_pipe_args);
        exit(EXIT_FAILURE);
      } else {
        waitpid(pid, NULL, 0);
      }
    }

    free_parsed_command(&parsed_command);
    printf("mini-bash $ ");
  }

  free(input);
  return EXIT_SUCCESS;
}