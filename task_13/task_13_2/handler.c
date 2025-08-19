#include "handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

mqd_t create_queue(const char* queue_name, int flags) {
  struct mq_attr attr;

  attr.mq_flags = 0;
  attr.mq_maxmsg = MAX_QUEUE_SIZE;
  attr.mq_msgsize = sizeof(message_t);
  attr.mq_curmsgs = 0;

  mqd_t queue_fd = mq_open(queue_name, flags, 0644, &attr);
  if (queue_fd == (mqd_t)-1) {
    perror("mq_open");
    return -1;
  }

  return queue_fd;
}

int send_message(mqd_t queue_fd, const message_t* msg) {
  if (mq_send(queue_fd, (const char*)msg, sizeof(message_t), 0) == -1) {
    perror("mq_send");
    return -1;
  }
  return 0;
}

int receive_message(mqd_t queue_fd, message_t* msg) {
  ssize_t bytes_read =
      mq_receive(queue_fd, (char*)msg, sizeof(message_t), NULL);
  if (bytes_read == -1) {
    perror("mq_receive");
    return -1;
  }
  return 0;
}

void close_and_unlink_queue(mqd_t queue_fd, const char* queue_name) {
  if (queue_fd != -1) {
    mq_close(queue_fd);
  }
  mq_unlink(queue_name);
}

client_info_t* add_client(client_info_t** head, const char* username,
                          pid_t pid) {
  client_info_t* new_client = malloc(sizeof(client_info_t));
  if (!new_client) {
    perror("malloc");
    return NULL;
  }

  strncpy(new_client->username, username, MAX_USERNAME_SIZE - 1);
  new_client->username[MAX_USERNAME_SIZE - 1] = '\0';

  snprintf(new_client->queue_name, sizeof(new_client->queue_name), "%s%d",
           CLIENT_QUEUE_PREFIX, pid);

  new_client->pid = pid;
  new_client->queue_fd = -1;
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

      if (current->queue_fd != -1) {
        close_and_unlink_queue(current->queue_fd, current->queue_name);
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

    if (temp->queue_fd != -1) {
      close_and_unlink_queue(temp->queue_fd, temp->queue_name);
    }

    free(temp);
  }
}
