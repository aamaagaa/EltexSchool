#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main() {
  sigset_t set;
  
  sigemptyset(&set);
  sigaddset(&set, SIGINT);

  sigprocmask(SIG_BLOCK, &set, NULL);
  printf("SIGINT заблокирован\n");

  while(1){
    sleep(1);
  }

  return 0;
}