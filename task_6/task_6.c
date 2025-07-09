#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ABONENTS 100
#define MAX_INFO_LEN 10

typedef struct Abonent {
  char name[10];
  char second_name[10];
  char tel[10];
  struct Abonent *next;
  struct Abonent *prev;
} Abonent;

Abonent *head = NULL;
int NumberOfAbonents = 0;

char *GetInfo() {
  char buff[256];
  scanf("%s", buff);
  char *info = malloc(MAX_INFO_LEN);
  if (!info) {
    perror("Out of memory!\n");
    exit(EXIT_FAILURE);
  }
  strncpy(info, buff, MAX_INFO_LEN);
  return info;
}

Abonent *CreateAbonent(char *name, char *second_name, char *tel) {
  Abonent *abonent = malloc(sizeof(Abonent));
  if (!abonent) {
    perror("Out of memory!\n");
    exit(EXIT_FAILURE);
  }
  snprintf(abonent->name, MAX_INFO_LEN, "%s", name);
  snprintf(abonent->second_name, MAX_INFO_LEN, "%s", second_name);
  snprintf(abonent->tel, MAX_INFO_LEN, "%s", tel);
  abonent->next = NULL;
  abonent->prev = NULL;
  return abonent;
}

void AddAbonent() {
  if (NumberOfAbonents == MAX_ABONENTS) {
    printf("Максимальное количество абонентов\n");
    return;
  }
  char *name, *second_name, *tel;
  printf("Введите имя: ");
  name = GetInfo();
  printf("Введите фамилию: ");
  second_name = GetInfo();
  printf("Введите телефон: ");
  tel = GetInfo();
  Abonent *abonent = CreateAbonent(name, second_name, tel);
  if (head == NULL) {
    head = abonent;
    NumberOfAbonents++;
    free(name);
    free(second_name);
    free(tel);
    return;
  }
  Abonent *temp = head;
  while (temp->next != NULL) {
    temp = temp->next;
  }
  temp->next = abonent;
  abonent->prev = temp;
  NumberOfAbonents++;
  free(name);
  free(second_name);
  free(tel);
}

void DeleteAbonent() {
  char *name, *second_name, *tel;
  printf("Введите имя: ");
  name = GetInfo();
  printf("Введите фамилию: ");
  second_name = GetInfo();
  printf("Введите телефон: ");
  tel = GetInfo();
  Abonent *temp = head;
  while (temp != NULL) {
    if (strcmp(name, temp->name) == 0 &&
        strcmp(second_name, temp->second_name) == 0 &&
        strcmp(tel, temp->tel) == 0) {
      if (temp == head) {
        head = temp->next;
      } else if (temp->next == NULL) {
        temp->prev->next = NULL;
      } else {
        temp->next->prev = temp->prev;
        temp->prev->next = temp->next;
      }
      free(temp);
      NumberOfAbonents--;
      free(name);
      free(second_name);
      free(tel);
      return;
    }
    temp = temp->next;
  }
  printf("Абонент не найден\n");
  free(name);
  free(second_name);
  free(tel);
}

void FindAbonent() {
  char *name;
  char found = 0;
  printf("Введите имя: ");
  name = GetInfo();
  Abonent *temp = head;
  while (temp != NULL) {
    if (strcmp(name, temp->name) == 0) {
      found = 1;
      printf("%-9s %-9s %-9s\n", temp->name, temp->second_name, temp->tel);
    }
    temp = temp->next;
  }
  if (found == 0) {
    printf("Абонент не найден.\n");
  }
  free(name);
}

void PrintAbonents() {
  if (head == NULL) {
    printf("Список абонентов пуст.\n");
    return;
  }
  Abonent *temp = head;
  printf("Имя      Фамилия   Телефон   \n");
  while (temp != NULL) {
    printf("%-9s %-9s %-9s\n", temp->name, temp->second_name, temp->tel);
    temp = temp->next;
  }
}

void FreeList() {
  while (head != NULL) {
    Abonent *temp;
    head = head->next;
    free(temp);
  }
}

void menu() {
  char option[256];
  do {
    printf("1) Добавить абонента\n");
    printf("2) Удалить абонента\n");
    printf("3) Поиск абонентов по имени\n");
    printf("4) Вывод всех записей\n");
    printf("5) Выход\n");
    scanf("%s", option);
    switch (option[0]) {
      case '1':
        AddAbonent();
        break;
      case '2':
        DeleteAbonent();
        break;
      case '3':
        FindAbonent();
        break;
      case '4':
        PrintAbonents();
        break;
      case '5':
        FreeList();
        return;
      default:
        continue;
    }
  } while (1);
}
int main() {
  menu();
  return 0;
}