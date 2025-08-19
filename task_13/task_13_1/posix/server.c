#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SERVER_Q "/server_q"
#define CLIENT_Q "/client_q"

struct attributes {
  long flags;
  long maxmsg;
  long msgsize;
};

int main() {
  mq_unlink(SERVER_Q);
  mq_unlink(CLIENT_Q);
  struct attributes attr;
  attr.flags = 0;
  attr.maxmsg = 10;
  attr.msgsize = 1024;
  char server_msg[] = "Hi!";
  size_t msg_size = sizeof(server_msg);
  char buf[1024];

  mqd_t mq_s = mq_open(SERVER_Q, O_CREAT | O_WRONLY | O_EXCL, 0666, &attr);
  if (mq_s == (mqd_t)-1) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }

  if (mq_send(mq_s, server_msg, msg_size, 1) == -1) {
    perror("mq_send");
    mq_close(mq_s);
    mq_unlink(SERVER_Q);
    exit(EXIT_FAILURE);
  }

  mqd_t mq_c = mq_open(CLIENT_Q, O_CREAT | O_RDONLY | O_EXCL, 0666, &attr);
  if (mq_c == (mqd_t)-1) {
    perror("mq_open");
    mq_unlink(CLIENT_Q);
    exit(EXIT_FAILURE);
  }

  ssize_t bytes_received = mq_receive(mq_c, buf, sizeof(buf), NULL);
  if (bytes_received == -1) {
    perror("mq_receive");
    mq_close(mq_c);
    mq_unlink(CLIENT_Q);
    exit(EXIT_FAILURE);
  }

  printf("%s\n", buf);

  mq_close(mq_s);
  mq_unlink(SERVER_Q);
  return EXIT_SUCCESS;
}