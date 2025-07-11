#include <stdio.h>

#include "../library/libcalc.h"
int GetFirstNum() {
  printf("Введите первое число: ");
  int x;
  scanf("%d", &x);
  return x;
}
int GetSecondNum() {
  printf("Введите второе число: ");
  int y;
  scanf("%d", &y);
  return y;
}
void PrintRes(float res) { printf("Результат: %.1f\n", (float)res); }
void Menu(char option) {
  int num1, num2;
  float res;
  printf("1) Сложение\n");
  printf("2) Вычитание\n");
  printf("3) Умножение\n");
  printf("4) Деление\n");
  printf("5) Выход\n");
  switch (option) {
    case '1':
      num1 = GetFirstNum();
      num2 = GetSecondNum();
      res = add(num1, num2);
      PrintRes(res);
      break;
    case '2':
      num1 = GetFirstNum();
      num2 = GetSecondNum();
      res = sub(num1, num2);
      PrintRes(res);
      break;
    case '3':
      num1 = GetFirstNum();
      num2 = GetSecondNum();
      res = mul(num1, num2);
      PrintRes(res);
      break;
    case '4':
      num1 = GetFirstNum();
      num2 = GetSecondNum();
      res = div(num1, num2);
      PrintRes(res);
      break;
    case '5':
      return;
    default:
      return;
  }
}

int main() {
  char option = 0;
  do {
    Menu(option);
    option = getchar();
  } while (option != '5');

  return 0;
}