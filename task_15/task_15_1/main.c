#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void signal_handler(int sig) { printf("Получен сигнал %d\n", sig); }

int main() {
  struct sigaction sa;

  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction(SIGUSR1, &sa, NULL) == -1) {
    perror("sigaction");
    return 1;
  }

  printf("Ожидание сигнала...\n");
  while (1) {
    sleep(1);
  }

  return 0;
}
