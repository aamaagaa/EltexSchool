#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "handler.h"

static int shm_fd = -1;
static chat_room_t* chat_room = NULL;
static sem_t* chat_sem = NULL;
static client_info_t* clients = NULL;
static pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void signal_handler(int sig) {
  printf("\nЗавершение работы сервера...\n");

  pthread_mutex_lock(&clients_mutex);
  free_client_list(clients);
  pthread_mutex_unlock(&clients_mutex);

  close_and_unlink_shm(shm_fd, chat_room, CHAT_SHM_NAME);
  close_and_unlink_sem(chat_sem, CHAT_SEM_NAME);
  exit(0);
}


message_t create_join_message(const char* username) {
  message_t msg = {.type = MSG_USER_JOINED, .client_pid = 0};
  strncpy(msg.username, username, MAX_USERNAME_SIZE);
  msg.username[MAX_USERNAME_SIZE - 1] = '\0';
  snprintf(msg.content, MAX_MESSAGE_SIZE, "%s joined the chat", username);
  return msg;
}

void add_client_to_room(chat_room_t* room, const char* username, pid_t pid) {
  if (sem_wait(chat_sem) == -1) {
    perror("sem_wait");
    return;
  }

  if (room->client_count < MAX_CLIENTS) {
    strncpy(room->clients[room->client_count].username, username, MAX_USERNAME_SIZE - 1);
    room->clients[room->client_count].username[MAX_USERNAME_SIZE - 1] = '\0';
    room->clients[room->client_count].pid = pid;
    room->clients[room->client_count].active = 1;
    room->client_count++;
  }

  if (sem_post(chat_sem) == -1) {
    perror("sem_post");
  }
}

void handle_client_join(const message_t* msg) {
  printf("Клиент %s (PID: %d) присоединился к чату\n", msg->username, msg->client_pid);

  pthread_mutex_lock(&clients_mutex);
  client_info_t* new_client = add_client(&clients, msg->username, msg->client_pid);
  if (new_client) {
    add_client_to_room(chat_room, msg->username, msg->client_pid);
  }
  pthread_mutex_unlock(&clients_mutex);

  message_t join_msg = create_join_message(msg->username);
  send_message(chat_room, chat_sem, &join_msg);
}

void handle_chat_message(const message_t* msg) {
  printf("[%s]: %s\n", msg->username, msg->content);\
}

int main() {
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  shm_fd = create_shared_memory(CHAT_SHM_NAME, O_CREAT | O_RDWR);
  if (shm_fd == -1) {
    fprintf(stderr, "Ошибка создания shared memory\n");
    return 1;
  }

  chat_room = map_shared_memory(shm_fd);
  if (!chat_room) {
    fprintf(stderr, "Ошибка отображения shared memory\n");
    close(shm_fd);
    return 1;
  }

  chat_sem = create_semaphore(CHAT_SEM_NAME, O_CREAT);
  if (!chat_sem) {
    fprintf(stderr, "Ошибка создания semaphore\n");
    close_and_unlink_shm(shm_fd, chat_room, CHAT_SHM_NAME);
    return 1;
  }

  memset(chat_room, 0, sizeof(chat_room_t));

  printf("Сервер запущен. Ожидание клиентов...\n");

  int processed_messages = 0;

  while (1) {
    if (sem_wait(chat_sem) == -1) {
      perror("sem_wait");
      usleep(100000);
      continue;
    }

    int current_count = chat_room->message_count;
    message_t temp_messages[MAX_MESSAGES];
    int new_msg_count = 0;

    for (int i = processed_messages; i < current_count; i++) {
      if (chat_room->messages[i].client_pid != 0) {
        temp_messages[new_msg_count] = chat_room->messages[i];
        new_msg_count++;
      }
    }
    
    processed_messages = current_count;

    if (sem_post(chat_sem) == -1) {
      perror("sem_post");
    }

    for (int i = 0; i < new_msg_count; i++) {
      switch (temp_messages[i].type) {
        case MSG_JOIN:
          handle_client_join(&temp_messages[i]);
          break;

        case MSG_CHAT:
          handle_chat_message(&temp_messages[i]);
          break;

        default:
          printf("Неизвестный тип сообщения: %d\n", temp_messages[i].type);
          break;
      }
    }

    usleep(100000);
  }

  return 0;
}
