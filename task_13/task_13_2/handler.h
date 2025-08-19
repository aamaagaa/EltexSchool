#ifndef HANDLER_H
#define HANDLER_H

#include <mqueue.h>
#include <sys/types.h>

#define MAX_MESSAGE_SIZE 256
#define MAX_USERNAME_SIZE 32
#define MAX_QUEUE_SIZE 10
#define SERVER_QUEUE_NAME "/chat_server_queue"
#define CLIENT_QUEUE_PREFIX "/chat_client_"

typedef enum { MSG_JOIN, MSG_CHAT, MSG_USER_JOINED } message_type_t;

typedef struct {
  message_type_t type;
  char username[MAX_USERNAME_SIZE];
  char content[MAX_MESSAGE_SIZE];
  pid_t client_pid;
} message_t;

typedef struct client_info {
  char username[MAX_USERNAME_SIZE];
  char queue_name[64];
  pid_t pid;
  mqd_t queue_fd;
  struct client_info* next;
} client_info_t;

mqd_t create_queue(const char* queue_name, int flags);
int send_message(mqd_t queue_fd, const message_t* msg);
int receive_message(mqd_t queue_fd, message_t* msg);
void close_and_unlink_queue(mqd_t queue_fd, const char* queue_name);

client_info_t* add_client(client_info_t** head, const char* username,
                          pid_t pid);
void remove_client(client_info_t** head, pid_t pid);
void free_client_list(client_info_t* head);

#endif
