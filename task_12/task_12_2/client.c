#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FIFO_PATH "/tmp/my_fifo"
#define BUF_SIZE 1024

int main(){
  int fd;
  char buffer[BUF_SIZE];

  fd = open(FIFO_PATH, O_RDONLY);
  if(fd == -1){
    perror("open");
    unlink(FIFO_PATH);
    exit(EXIT_FAILURE);
  }

  read(fd, buffer, BUF_SIZE);
  printf("%s\n", buffer);
  close(fd);
  if (unlink(FIFO_PATH)) {
    perror("unlink");
    exit(EXIT_FAILURE);
  }
  return EXIT_SUCCESS;
}