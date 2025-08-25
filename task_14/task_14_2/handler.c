#include "handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int create_shared_memory(const char* shm_name, int flags) {
  int shm_fd = shm_open(shm_name, flags, 0644);
  if (shm_fd == -1) {
    perror("shm_open");
    return -1;
  }

  if (flags & O_CREAT) {
    if (ftruncate(shm_fd, sizeof(chat_room_t)) == -1) {
      perror("ftruncate");
      close(shm_fd);
      return -1;
    }
  }

  return shm_fd;
}

chat_room_t* map_shared_memory(int shm_fd) {
  chat_room_t* room = mmap(NULL, sizeof(chat_room_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (room == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }
  return room;
}

sem_t* create_semaphore(const char* sem_name, int flags) {
  sem_t* sem = sem_open(sem_name, flags, 0644, 1);
  if (sem == SEM_FAILED) {
    perror("sem_open");
    return NULL;
  }
  return sem;
}

int send_message(chat_room_t* room, sem_t* sem, const message_t* msg) {
  if (sem_wait(sem) == -1) {
    perror("sem_wait");
    return -1;
  }

  if (room->message_count < MAX_MESSAGES) {
    room->messages[room->message_count] = *msg;
    room->message_count++;
    room->last_message_id++;
  } else {
    for (int i = 0; i < MAX_MESSAGES - 1; i++) {
      room->messages[i] = room->messages[i + 1];
    }
    room->messages[MAX_MESSAGES - 1] = *msg;
    room->last_message_id++;
  }

  if (sem_post(sem) == -1) {
    perror("sem_post");
    return -1;
  }

  return 0;
}

int get_new_messages(chat_room_t* room, sem_t* sem, message_t* msgs, int* last_id) {
  if (sem_wait(sem) == -1) {
    perror("sem_wait");
    return -1;
  }

  int new_count = 0;
  
  if (*last_id == 0) {
    new_count = room->message_count;
    for (int i = 0; i < new_count; i++) {
      msgs[i] = room->messages[i];
    }
    *last_id = room->last_message_id;
  } else if (room->last_message_id > *last_id) {
    int messages_to_get = room->last_message_id - *last_id;
    if (messages_to_get > room->message_count) {
      new_count = room->message_count;
      for (int i = 0; i < new_count; i++) {
        msgs[i] = room->messages[i];
      }
    } else {
      new_count = messages_to_get;
      int start_idx = room->message_count - messages_to_get;
      for (int i = 0; i < new_count; i++) {
        msgs[i] = room->messages[start_idx + i];
      }
    }
    *last_id = room->last_message_id;
  }

  if (sem_post(sem) == -1) {
    perror("sem_post");
    return -1;
  }

  return new_count;
}

void close_and_unlink_shm(int shm_fd, chat_room_t* room, const char* shm_name) {
  if (room) {
    munmap(room, sizeof(chat_room_t));
  }
  if (shm_fd != -1) {
    close(shm_fd);
  }
  shm_unlink(shm_name);
}

void close_and_unlink_sem(sem_t* sem, const char* sem_name) {
  if (sem) {
    sem_close(sem);
  }
  sem_unlink(sem_name);
}

client_info_t* add_client(client_info_t** head, const char* username, pid_t pid) {
  client_info_t* new_client = malloc(sizeof(client_info_t));
  if (!new_client) {
    perror("malloc");
    return NULL;
  }

  strncpy(new_client->username, username, MAX_USERNAME_SIZE - 1);
  new_client->username[MAX_USERNAME_SIZE - 1] = '\0';
  new_client->pid = pid;
  new_client->next = *head;

  *head = new_client;
  return new_client;
}

void remove_client(client_info_t** head, pid_t pid) {
  client_info_t* current = *head;
  client_info_t* prev = NULL;

  while (current) {
    if (current->pid == pid) {
      if (prev) {
        prev->next = current->next;
      } else {
        *head = current->next;
      }
      free(current);
      return;
    }
    prev = current;
    current = current->next;
  }
}

void free_client_list(client_info_t* head) {
  while (head) {
    client_info_t* temp = head;
    head = head->next;
    free(temp);
  }
}
