#include "handler.h"

int main() {
  connect_to_server();
  sem_wait(sem);
  printf("%s\n", shm_data->message);
  sem_post(sem);

  sem_wait(sem);
  strcpy(shm_data->message, "Hello!");
  sem_post(sem);

  return EXIT_SUCCESS;
}