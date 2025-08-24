#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main() {
  sigset_t set;
  int sig;
  
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);

  sigprocmask(SIG_BLOCK, &set, NULL);

  while(1){
    printf("Ожидание сигнала SIGUSR1...\n");
    if(sigwait(&set, &sig) == 0){
      printf("Получен сигнал %d\n", sig);
    } else{
      perror("sigwait");
      break;
    }
  }

  return 0;
}