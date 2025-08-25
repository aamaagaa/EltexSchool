#include "handler.h"

int create_semaphore(key_t key, int initial_value) {
  int semid;
  
  if ((semid = semget(key, 1, IPC_CREAT | 0666)) == -1) {
      return -1;
  }
  
  if (semctl(semid, 0, SETVAL, initial_value) == -1) {
      return -1;
  }
  
  return semid;
}

void sem_wait(int semid, int sem_num) {
  struct sembuf lock = {sem_num, -1, 0};
  
  if (semop(semid, &lock, 1) == -1) {
      perror("semop");
      exit(1);
  }
}

void sem_signal(int semid, int sem_num) {
  struct sembuf unlock = {sem_num, 1, 0};
  if (semop(semid, &unlock, 1) == -1) {
      perror("semop");
      exit(1);
  }
}



