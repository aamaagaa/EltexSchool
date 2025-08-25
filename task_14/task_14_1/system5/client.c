#include "handler.h"

#define MSG = "Hello!"

int main() {
  key_t key = ftok("newkey", 1);
  int shm_id = shmget(key, 0, 0);
  if (shm_id == -1) {
    perror("shmget");
    return EXIT_FAILURE;
  }

  struct shared_data * shm_ptr; 
  if ((shm_ptr = (struct shared_data *)shmat(shm_id, NULL, 0)) == (struct shared_data *) -1) {
      perror("shmat");
      return EXIT_FAILURE;
  }

  int sem_id = semget(key, 0, 0);
  
  sem_wait(sem_id, 0);
  printf("%s\n", shm_ptr->message);
  sem_signal(sem_id, 0);

  sem_wait(sem_id, 0);
  strcpy(shm_ptr->message, "Hello!");
  sem_signal(sem_id, 0);

  if(shmdt(shm_ptr) == -1){
    perror("shmdt");
  }

  return EXIT_SUCCESS;
}