#ifndef HANDLER_H
#define HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

#define SHM_NAME "/posix_shm"
#define SEM_NAME "/sem"
#define SHM_SIZE 1024

extern sem_t *sem;
extern int shm_fd;

struct shared_data {
    char message[SHM_SIZE];
};

extern struct shared_data *shm_data;

void create_shared_memory(void);
void create_semaphore(void);
int connect_to_server(void);
void cleanup_resources(void);

#endif