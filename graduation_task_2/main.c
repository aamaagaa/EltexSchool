#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_DRIVERS 10
#define MAX_BUFFER 100

typedef struct {
    pid_t pid;
    int status;
    int timer;
    int pipe_fd[2];
} driver_t;

driver_t drivers[MAX_DRIVERS];
int driver_count = 0;

void create_driver() {
    if (driver_count >= MAX_DRIVERS) {
        return;
    }

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        close(pipe_fd[1]);
        
        char buffer[MAX_BUFFER];
        int is_busy = 0;
        int task_time = 0;
        time_t task_start = 0;

        while (1) {
            if (read(pipe_fd[0], buffer, sizeof(buffer)) > 0) {
                if (strncmp(buffer, "TASK", 4) == 0) {
                    int _time = atoi(buffer + 5);
                    if (is_busy) {
                        int time_left = task_time - (time(NULL) - task_start);
                        if (time_left < 0) time_left = 0;
                        printf("Busy %d\n", time_left);
                    } else {
                        is_busy = 1;
                        task_time = _time;
                        task_start = time(NULL);
                        printf("Task started for %d seconds\n", _time);
                    }
                }
                else if (strncmp(buffer, "STATUS", 6) == 0) {
                    if (is_busy) {
                        int time_left = task_time - (time(NULL) - task_start);
                        if (time_left < 0) {
                            is_busy = 0;
                            printf("%-10s", "Available");
                            fflush(stdout);
                        } else {
                            printf("%-10s", "Busy");
                            fflush(stdout);
                        }
                    } else {
                        printf("%-10s", "Available");
                        fflush(stdout);
                    }
                }
                else if (strncmp(buffer, "EXIT", 4) == 0) {
                    break;
                }
            }

            if (is_busy && (time(NULL) - task_start >= task_time)) {
                is_busy = 0;
            }
        }
        close(pipe_fd[0]);
        exit(0);
    }
    else {
        close(pipe_fd[0]);
        drivers[driver_count].pid = pid;
        drivers[driver_count].status = 0;
        drivers[driver_count].timer = 0;
        drivers[driver_count].pipe_fd[0] = pipe_fd[0];
        drivers[driver_count].pipe_fd[1] = pipe_fd[1];
        driver_count++;
        printf("Driver created! PID: %d\n", pid);
    }
}

void send_task(char* pid_str, char* timer_str) {
    pid_t pid = atoi(pid_str);
    int timer = atoi(timer_str);

    for (int i = 0; i < driver_count; i++) {
        if (drivers[i].pid == pid) {
            char command[MAX_BUFFER];
            snprintf(command, sizeof(command), "TASK %d", timer);
            write(drivers[i].pipe_fd[1], command, strlen(command) + 1);
            return;
        }
    }
    printf("Driver %d not found\n", pid);
}

void get_status(char* pid_str) {
    pid_t pid = atoi(pid_str);

    for (int i = 0; i < driver_count; i++) {
        if (drivers[i].pid == pid) {
            write(drivers[i].pipe_fd[1], "STATUS", 7);
            return;
        }
    }
    printf("Driver %d not found\n", pid);
}

void get_drivers() {
    printf("Drivers (%d):\n", driver_count);
    for (int i = 0; i < driver_count; i++) {
        printf("%-10d", drivers[i].pid);
        write(drivers[i].pipe_fd[1], "STATUS", 7);
    }
    printf("\n");
}

void print_commands(){
  printf("Commands:\n");
  printf("create_driver - создать новый процесс\n");
  printf("send_task <pid> <time> - создать задачу для driver номер <pid> и занять его на <task_timer> секунд\n");
  printf("get_status <pid> - показать статус driver с номером <pid>\n");
  printf("get_drivers - показать статусы и pid всех drivers\n");
  printf("commands - показать все комманды\n");
  printf("exit - выход\n");
}

void cleanup() {
    for (int i = 0; i < driver_count; i++) {
        write(drivers[i].pipe_fd[1], "EXIT", 5);
        close(drivers[i].pipe_fd[1]);
    }
    
    while (wait(NULL) > 0);
}

int main() {
    char input[MAX_BUFFER];
    char command[20], arg1[20], arg2[20];

    print_commands();

    while (1) {
        fflush(stdout);
        
        if (!fgets(input, sizeof(input), stdin)) break;
        
        input[strcspn(input, "\n")] = 0;
        
        int args = sscanf(input, "%19s %19s %19s", command, arg1, arg2);
        
        if (strcmp(command, "create_driver") == 0) {
            create_driver();
        }
        else if (strcmp(command, "send_task") == 0 && args == 3) {
            send_task(arg1, arg2);
        }
        else if (strcmp(command, "get_status") == 0 && args == 2) {
            get_status(arg1);
        }
        else if (strcmp(command, "get_drivers") == 0) {
            get_drivers();
        }
        else if (strcmp(command, "commands") == 0) {
            print_commands();
        }
        else if (strcmp(command, "exit") == 0) {
            break;
        }
        else if(strlen(input) == 0){
          continue;
        }
        else {
            printf("Unknown command\n");
        }
    }

    cleanup();
    return 0;
}