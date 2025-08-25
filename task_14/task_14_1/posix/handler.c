#include "handler.h"

sem_t *sem = SEM_FAILED;
int shm_fd = -1;
struct shared_data *shm_data = NULL;

void create_shared_memory(void) {
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    
    if (ftruncate(shm_fd, sizeof(struct shared_data)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    
    shm_data = (struct shared_data *)mmap(NULL, sizeof(struct shared_data), 
                                         PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_data == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    memset(shm_data, 0, sizeof(struct shared_data));
}

void create_semaphore(void) {
  sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0666, 1);
  if (sem == SEM_FAILED) {
      perror("sem_open");
      exit(EXIT_FAILURE);
  }
}

int connect_to_server(void) {
    shm_fd = shm_open(SHM_NAME, O_RDWR, 0);
    if (shm_fd == -1) {
      perror("shm_open");
        return 0;
    }
    
    shm_data = (struct shared_data *)mmap(NULL, sizeof(struct shared_data), 
                                         PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_data == MAP_FAILED) {
        perror("mmap");
        return 0;
    }
    
    sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 0;
    }
    return 1;
}

void cleanup_resources(void) {
    sem_close(sem);
    munmap(shm_data, sizeof(struct shared_data));
    close(shm_fd);
    sem_unlink(SEM_NAME);
    shm_unlink(SHM_NAME);
}


