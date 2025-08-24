#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Использование: %s <имя процесса>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  char command[256];

  snprintf(command, sizeof(command), "pkill -SIGUSR1 %s", argv[1]);

  printf("%s\n", command);

  system(command);

  return 0;
}