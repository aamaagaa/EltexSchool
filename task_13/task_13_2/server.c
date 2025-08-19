#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "handler.h"

#define MAX_MESSAGES 1024

static mqd_t server_queue = -1;
static client_info_t* clients = NULL;
static pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void signal_handler(int sig) {
  printf("\nЗавершение работы сервера...\n");

  pthread_mutex_lock(&clients_mutex);
  free_client_list(clients);
  pthread_mutex_unlock(&clients_mutex);

  close_and_unlink_queue(server_queue, SERVER_QUEUE_NAME);
  exit(0);
}

void broadcast_message(const message_t* msg) {
  pthread_mutex_lock(&clients_mutex);

  client_info_t* current = clients;
  while (current) {
    if (current->queue_fd != -1) {
      send_message(current->queue_fd, msg);
    }
    current = current->next;
  }

  pthread_mutex_unlock(&clients_mutex);
}

void send_chat(const message_t history[], int msg_count, mqd_t client_fd) {
  if (client_fd == -1) {
    fprintf(stderr, "Invalid client queue descriptor\n");
    return;
  }

  for (int i = 0; i < msg_count; i++) {
    if (history[i].content[0] != '\0') {
      if (send_message(client_fd, &history[i]) == -1) {
        perror("Failed to send chat history");
        break;
      }
    }
  }
}

message_t create_join_message(const char* username) {
  message_t msg = {.type = MSG_USER_JOINED, .client_pid = 0};
  strncpy(msg.username, username, MAX_USERNAME_SIZE);
  msg.username[MAX_USERNAME_SIZE - 1] = '\0';
  snprintf(msg.content, MAX_MESSAGE_SIZE, "%s joined the chat", username);
  return msg;
}

void add_to_history(message_t* history, int* count, const message_t* msg) {
  if (*count >= MAX_MESSAGES) {
    memmove(history, history + 1, (MAX_MESSAGES - 1) * sizeof(message_t));
    *count = MAX_MESSAGES - 1;
  }
  history[*count] = *msg;
  (*count)++;
}

void handle_client_join(const message_t* msg, message_t* history,
                        int* msg_count) {
  printf("Клиент %s (PID: %d) присоединился к чату\n", msg->username,
         msg->client_pid);

  pthread_mutex_lock(&clients_mutex);
  message_t join_msg = create_join_message(msg->username);

  client_info_t* new_client =
      add_client(&clients, msg->username, msg->client_pid);
  if (new_client) {
    new_client->queue_fd = create_queue(new_client->queue_name, O_WRONLY);
    if (new_client->queue_fd == -1) {
      printf("Ошибка открытия очереди клиента %s\n", msg->username);
      remove_client(&clients, msg->client_pid);
      pthread_mutex_unlock(&clients_mutex);
      return;
    }

    send_chat(history, *msg_count, new_client->queue_fd);
  }
  add_to_history(history, msg_count, &join_msg);
  pthread_mutex_unlock(&clients_mutex);
  broadcast_message(&join_msg);
}

void handle_chat_message(const message_t* msg, message_t* history,
                         int* msg_count) {
  history[*msg_count] = *msg;
  (*msg_count)++;
  printf("[%s]: %s\n", msg->username, msg->content);

  broadcast_message(msg);
}

int main() {
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  server_queue = create_queue(SERVER_QUEUE_NAME, O_CREAT | O_RDONLY);

  printf("Сервер запущен. Ожидание клиентов...\n");

  message_t msg;
  message_t chat_history[MAX_MESSAGES];
  int msg_counter = 0;
  while (1) {
    if (receive_message(server_queue, &msg) == 0) {
      switch (msg.type) {
        case MSG_JOIN:
          handle_client_join(&msg, chat_history, &msg_counter);
          break;

        case MSG_CHAT:
          handle_chat_message(&msg, chat_history, &msg_counter);
          break;

        default:
          printf("Неизвестный тип сообщения: %d\n", msg.type);
          break;
      }
    }
  }

  return 0;
}
