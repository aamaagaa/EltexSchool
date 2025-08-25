#include "handler.h"

int main() {
  sem_unlink(SEM_NAME);
  shm_unlink(SHM_NAME);
  create_shared_memory();
  create_semaphore();

  sem_wait(sem);
  strcpy(shm_data->message, "Hi!");
  sem_post(sem);

  sleep(5);

  sem_wait(sem);
  printf("%s\n", shm_data->message);
  sem_post(sem);
  
  cleanup_resources();
  return EXIT_SUCCESS;
}