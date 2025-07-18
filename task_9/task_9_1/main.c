#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main() {
  FILE* file = fopen("output.txt", "w+");
  if (!file) {
    perror("File error\n");
    exit(EXIT_FAILURE);
  }
  fputs("String from file", file);
  fflush(file);

  struct stat st;
  stat("output.txt", &st);
  int size = st.st_size;
  for (int i = -1; i >= -size; i--) {
    fseek(file, i, SEEK_END);
    char ch = fgetc(file);
    if (ch == EOF) break;
    printf("%c", ch);
  }
  fclose(file);
  return 0;
}
