#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "handler.h"

#define STATUS_WIN_WIDTH 20
#define INPUT_WIN_HEIGHT 3

static mqd_t server_queue = -1;
static mqd_t client_queue = -1;
static char client_queue_name[64];
static char username[MAX_USERNAME_SIZE];
static pthread_t receive_thread;
static int running = 1;

static WINDOW* chat_win;
static WINDOW* input_win;
static WINDOW* status_win;

void init_interface() {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);

  int height, width;
  getmaxyx(stdscr, height, width);

  chat_win = newwin(height - INPUT_WIN_HEIGHT - 1, width - STATUS_WIN_WIDTH - 1, 0, 0);
  status_win = newwin(height - INPUT_WIN_HEIGHT - 1, STATUS_WIN_WIDTH, 0, width - STATUS_WIN_WIDTH);
  input_win = newwin(INPUT_WIN_HEIGHT, width, height - INPUT_WIN_HEIGHT, 0);

  scrollok(chat_win, TRUE);
  box(input_win, 0, 0);

  wrefresh(input_win);
}

void cleanup_interface() {
  if (chat_win) delwin(chat_win);
  if (input_win) delwin(input_win);
  if (status_win) delwin(status_win);
  endwin();
}

void add_message_to_chat(const char* message) {
  wprintw(chat_win, "%s\n", message);
  wrefresh(chat_win);
}

void add_user_to_chat(const char* username) {
  wprintw(status_win, "%s\n", username);
  wrefresh(status_win);
}

void* receive_messages(void* arg) {
  message_t msg;
  char display_msg[MAX_MESSAGE_SIZE + MAX_USERNAME_SIZE + 10];

  while (running) {
    if (receive_message(client_queue, &msg) == 0) {
      switch (msg.type) {
        case MSG_CHAT:
          snprintf(display_msg, sizeof(display_msg), "[%s]: %s", msg.username,
                   msg.content);
          add_message_to_chat(display_msg);
          break;

        case MSG_USER_JOINED:
          snprintf(display_msg, sizeof(display_msg), "%s", msg.content);
          add_message_to_chat(display_msg);
          add_user_to_chat(msg.username);
          break;

        default:
          break;
      }
    }
  }

  return NULL;
}

void send_chat_message(const char* content) {
  message_t msg;
  msg.type = MSG_CHAT;
  strncpy(msg.username, username, MAX_USERNAME_SIZE - 1);
  msg.username[MAX_USERNAME_SIZE - 1] = '\0';
  strncpy(msg.content, content, MAX_MESSAGE_SIZE - 1);
  msg.content[MAX_MESSAGE_SIZE - 1] = '\0';
  msg.client_pid = getpid();

  send_message(server_queue, &msg);
}

int join_chat() {
  message_t msg;
  msg.type = MSG_JOIN;
  strncpy(msg.username, username, MAX_USERNAME_SIZE - 1);
  msg.username[MAX_USERNAME_SIZE - 1] = '\0';
  msg.content[0] = '\0';
  msg.client_pid = getpid();

  return send_message(server_queue, &msg);
}

void signal_handler(int sig) {
  running = 0;
  cleanup_interface();

  if (client_queue != -1) {
    close_and_unlink_queue(client_queue, client_queue_name);
  }
  if (server_queue != -1) {
    mq_close(server_queue);
  }

  exit(0);
}

int main() {
  printf("Enter your username: ");
  if (fgets(username, MAX_USERNAME_SIZE, stdin) == NULL) {
    fprintf(stderr, "Ошибка ввода имени\n");
    return 1;
  }

  username[strcspn(username, "\n")] = '\0';

  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  snprintf(client_queue_name, sizeof(client_queue_name), "%s%d",
           CLIENT_QUEUE_PREFIX, getpid());

  client_queue = create_queue(client_queue_name, O_CREAT | O_RDONLY);
  if (client_queue == -1) {
    fprintf(stderr, "Ошибка создания очереди клиента\n");
    return 1;
  }

  server_queue = create_queue(SERVER_QUEUE_NAME, O_WRONLY);
  if (server_queue == -1) {
    fprintf(stderr, "Ошибка подключения к серверу\n");
    close_and_unlink_queue(client_queue, client_queue_name);
    return 1;
  }

  init_interface();

  if (pthread_create(&receive_thread, NULL, receive_messages, NULL) != 0) {
    fprintf(stderr, "Ошибка создания потока\n");
    cleanup_interface();
    close_and_unlink_queue(client_queue, client_queue_name);
    mq_close(server_queue);
    return 1;
  }

  if (join_chat() != 0) {
    fprintf(stderr, "Ошибка присоединения к чату\n");
    close_and_unlink_queue(client_queue, client_queue_name);
    mq_close(server_queue);
    return 1;
  }

  char input_buffer[MAX_MESSAGE_SIZE];
  int ch;
  int input_pos = 0;

  while (running) {
    mvwprintw(input_win, 1, 1, "Message: %s", input_buffer);
    wclrtoeol(input_win);
    box(input_win, 0, 0);
    wrefresh(input_win);
    wrefresh(status_win);
    wrefresh(chat_win);

    ch = getch();

    switch (ch) {
      case 27:
        running = 0;
        break;

      case '\n':
      case '\r':
      case KEY_ENTER:
        if (input_pos > 0) {
          input_buffer[input_pos] = '\0';
          send_chat_message(input_buffer);
          input_pos = 0;
          input_buffer[0] = '\0';
        }
        break;

      case KEY_BACKSPACE:
      case 127:
      case '\b':
        if (input_pos > 0) {
          input_pos--;
          input_buffer[input_pos] = '\0';
        }
        break;

      default:
        if (ch >= 32 && ch <= 126 && input_pos < MAX_MESSAGE_SIZE - 1) {
          input_buffer[input_pos] = ch;
          input_pos++;
          input_buffer[input_pos] = '\0';
        }
        break;
    }
  }

  pthread_cancel(receive_thread);
  pthread_join(receive_thread, NULL);

  cleanup_interface();
  close_and_unlink_queue(client_queue, client_queue_name);
  mq_close(server_queue);

  printf("Выход из чата.\n");
  return 0;
}
