#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define FIFO_PATH "/tmp/my_fifo"

int main(){
  int fd;
  char message[] = "Hi!";
  if(mkfifo(FIFO_PATH, 0666) == -1){
    if(errno != EEXIST){
    perror("mkfifo");
    exit(EXIT_FAILURE);
    }
  }

  fd = open(FIFO_PATH, O_WRONLY);
  if(fd == -1){
    perror("open");
    unlink(FIFO_PATH);
    exit(EXIT_FAILURE);
    }

  write(fd, message, strlen(message) + 1);

  close(fd);
  return EXIT_SUCCESS;
}