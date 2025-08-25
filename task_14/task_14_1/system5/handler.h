#ifndef HANDLER_H
#define HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define SHM_SIZE 1024
struct shared_data {
    char message[SHM_SIZE];
};
int create_semaphore(key_t key, int initial_value);
void sem_wait(int semid, int sem_num);
void sem_signal(int semid, int sem_num);

#endif