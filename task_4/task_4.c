#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ABONENTS 100
#define MAX_INFO_LEN 10

struct Abonent {
  char name[10];
  char second_name[10];
  char tel[10];
};

struct Abonent abonents[100];
int NumberOfAbonents = 0;

char *GetInfo() {
  char buff[256];
  scanf("%s", buff);
  char *info = malloc(MAX_INFO_LEN);
  strncpy(info, buff, MAX_INFO_LEN);
  return info;
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
  snprintf(abonents[NumberOfAbonents].name, MAX_INFO_LEN, "%s", name);
  snprintf(abonents[NumberOfAbonents].second_name, MAX_INFO_LEN, "%s",
           second_name);
  snprintf(abonents[NumberOfAbonents].tel, MAX_INFO_LEN, "%s", tel);
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
  for (int i = 0; i < NumberOfAbonents; i++) {
    if (strcmp(name, abonents[i].name) == 0 &&
        strcmp(second_name, abonents[i].second_name) == 0 &&
        strcmp(tel, abonents[i].tel) == 0) {
      for (int j = i + 1; j < NumberOfAbonents; j++) {
        strcpy(abonents[j - 1].name, abonents[j].name);
        strcpy(abonents[j - 1].second_name, abonents[j].second_name);
        strcpy(abonents[j - 1].tel, abonents[j].tel);
      }
      NumberOfAbonents--;
      free(name);
      free(second_name);
      free(tel);
      return;
    }
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
  for (int i = 0; i < NumberOfAbonents; i++) {
    if (strcmp(name, abonents[i].name) == 0) {
      found = 1;
      printf("%-9s %-9s %-9s\n", abonents[i].name, abonents[i].second_name,
             abonents[i].tel);
    }
  }
  if (found == 0) {
    printf("Абонент не найден.\n");
  }
  free(name);
}
void PrintAbonents() {
  if (NumberOfAbonents == 0) {
    printf("Список абонентов пуст.\n");
    return;
  }
  printf("Имя      Фамилия   Телефон   \n");
  for (int i = 0; i < NumberOfAbonents; i++) {
    printf("%-9s %-9s %-9s\n", abonents[i].name, abonents[i].second_name,
           abonents[i].tel);
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