#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct msg_buf {
  long mtype;
  char mtext[100];
};

int main() {
  key_t key = ftok("newkey", 1);
  int msg_id = msgget(key, 0666 | IPC_CREAT);
  if (msg_id == -1) {
    perror("msgget");
    return EXIT_FAILURE;
  }

  struct msg_buf msg;

  if (msgrcv(msg_id, &msg, sizeof(msg.mtext), 1, 0) == -1) {
    perror("msgrcv");
    return EXIT_FAILURE;
  } else {
    printf("%s\n", msg.mtext);
  }

  msg.mtype = 2;
  const char* client_msg = "Hello!";
  strncpy(msg.mtext, client_msg, strlen(client_msg));
  msg.mtext[strlen(msg.mtext)] = '\0';

  if (msgsnd(msg_id, &msg, sizeof(msg.mtext), 0) == -1) {
    perror("msgsnd");
    msgctl(msg_id, IPC_RMID, NULL);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}