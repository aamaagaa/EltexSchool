#include <stdio.h>
#include <stdlib.h>

int ReplaceThirdByte(int num, int val) {
  char* p = (char*)&num;
  p += 2;
  *p = val;
  return num;
}

void PrintArray() {
  const size_t arr_size = 10;
  int* arr = malloc(sizeof(int) * arr_size);
  int* beg = arr;
  for (int i = 0; i < arr_size; i++) {
    *arr = i + 1;
    printf("%d ", *arr);
    arr += 1;
  }
  printf("\n");
  free(beg);
}

char* FindSubstr(char* str, char* substr) {
  char* substr_beg = substr;
  char* str_beg = str;
  char* ans = NULL;
  while (*str != '\0' && *substr != '\0') {
    if (*str == *substr) {
      ans = str;
      while (*str != '\0' && *substr != '\0') {
        if (*str != *substr) {
          str = ans + 1;
          substr = substr_beg;
          ans = NULL;
          break;
        }
        str++;
        substr++;
      }
    } else {
      ans = NULL;
      str++;
    }
  }

  if (*substr != '\0') return NULL;
  return ans;
}
int main() {
  int num, val;
  printf("1. Поменять в целом положительном числе значение третьего байта\n");
  printf("Введите число: ");
  scanf("%d", &num);

  printf("Введите значение третьего байта(0-255): ");
  do {
    scanf("%d", &val);
  } while (val <= 0 && val >= 256);
  num = ReplaceThirdByte(num, val);
  printf("Результат: %d\n", num);

  printf("\n2. Напечатать “Результат: 12.0”.\n");
  {
    float x = 5.0;
    printf("x = %f, ", x);
    float y = 6.0;
    printf("y = %f\n", y);
    float* xp =
        &y;  // TODO: отредактируйте эту строку, и только правую часть уравнения
    float* yp = &y;
    printf("Результат: %f\n", *xp + *yp);
  }

  printf(
      "\n3. Используйте указатель для вывода элементов массива на консоль\n");
  PrintArray();

  printf("\n4. Найти подстроку в строке\n");
  static char str[256] = {}, substr[256] = {};
  printf("Введите строку: ");
  scanf("%s", str);
  printf("Введите подстроку: ");
  scanf("%s", substr);

  char* ans = FindSubstr(str, substr);
  if (ans != NULL)
    printf("%s\n", ans);
  else
    printf("NULL");

  return 0;
}