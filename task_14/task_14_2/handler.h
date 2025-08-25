#ifndef HANDLER_H
#define HANDLER_H

#include <sys/types.h>
#include <semaphore.h>

#define MAX_MESSAGE_SIZE 256
#define MAX_USERNAME_SIZE 32
#define MAX_CLIENTS 10
#define MAX_MESSAGES 1024
#define CHAT_SHM_NAME "/chat_room_shm"
#define CHAT_SEM_NAME "/chat_room_sem"

typedef enum { MSG_JOIN, MSG_CHAT, MSG_USER_JOINED } message_type_t;

typedef struct {
  message_type_t type;
  char username[MAX_USERNAME_SIZE];
  char content[MAX_MESSAGE_SIZE];
  pid_t client_pid;
} message_t;

typedef struct {
  char username[MAX_USERNAME_SIZE];
  pid_t pid;
  int active;
} client_t;

typedef struct {
  client_t clients[MAX_CLIENTS];
  message_t messages[MAX_MESSAGES];
  int client_count;
  int message_count;
  int last_message_id;
} chat_room_t;

typedef struct client_info {
  char username[MAX_USERNAME_SIZE];
  pid_t pid;
  struct client_info* next;
} client_info_t;

int create_shared_memory(const char* shm_name, int flags);
chat_room_t* map_shared_memory(int shm_fd);
sem_t* create_semaphore(const char* sem_name, int flags);
int send_message(chat_room_t* room, sem_t* sem, const message_t* msg);
int get_new_messages(chat_room_t* room, sem_t* sem, message_t* msgs, int* last_id);
void close_and_unlink_shm(int shm_fd, chat_room_t* room, const char* shm_name);
void close_and_unlink_sem(sem_t* sem, const char* sem_name);

client_info_t* add_client(client_info_t** head, const char* username, pid_t pid);
void remove_client(client_info_t** head, pid_t pid);
void free_client_list(client_info_t* head);

#endif
