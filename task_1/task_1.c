#include <stdio.h>
#include <stdlib.h>

char* ConvertDecToBin(int num) {
  size_t size_of_bin = sizeof(int) * 8;
  char* bin_str = malloc(size_of_bin + 1);

  for (int i = 0; i < size_of_bin; i++) {
    if ((num & (1 << i))) {
      bin_str[size_of_bin - i - 1] = '1';
    } else {
      bin_str[size_of_bin - i - 1] = '0';
    }
  }
  return bin_str;
}

int ReplaceThirdByte(int num, int val) {
  val = val << 16;
  int mask = ~(255 << 16);
  num = (num & mask) | val;
  return num;
}

char CountOnes(char* bin_str) {
  size_t size_of_bin = sizeof(int) * 8;
  char ones = 0;
  for (int i = 0; i < size_of_bin; i++) {
    ones += *bin_str - '0';
    bin_str++;
  }
  return ones;
}

int main() {
  int num;
  printf("Введите число: ");
  scanf("%d", &num);

  char* bin_str = ConvertDecToBin(num);
  printf("1. 2. Двоичное представление: %s\n", bin_str);

  printf("3. Колличество единиц: %d\n", CountOnes(bin_str));

  if (num >= 0) {
    int replace_num;
    printf("Введите число для замены (от 0 до 255): ");
    do {
      scanf("%d", &replace_num);
    } while (replace_num < 0 && replace_num >= 256);
    int result_num = ReplaceThirdByte(num, replace_num);
    char* result_str = ConvertDecToBin(result_num);
    printf("4. Число после замены: %s\n", result_str);
    free(result_str);
  }

  free(bin_str);

  return 0;
}