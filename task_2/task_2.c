#include <complex.h>
#include <stdio.h>
#include <stdlib.h>

#define N 5

int** CreateMatrix() {
  int** matrix = (int**)malloc(N * sizeof(int*));
  int val = 1;
  for (int i = 0; i < N; i++) {
    matrix[i] = (int*)malloc(N * sizeof(int));
    for (int j = 0; j < N; j++) {
      matrix[i][j] = val;
      val++;
    }
  }
  return matrix;
}

void FreeMatrix(int** matrix) {
  for (int i = 0; i < N; i++) {
    free(matrix[i]);
  }
  free(matrix);
}

void PrintMatrix(int** matrix) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      printf("%3d ", matrix[i][j]);
    }
    printf("\n");
  }
}

int* CreateArray() {
  int* array = (int*)malloc(N * sizeof(int));
  for (int i = 0; i < N; i++) {
    array[i] = i + 1;
  }
  return array;
}

int* ReverseArray(int* arr) {
  int half_size = N / 2;
  for (int i = 0; i < half_size; i++) {
    int temp = arr[i];
    arr[i] = arr[N - i - 1];
    arr[N - i - 1] = temp;
  }
  return arr;
}

void PrintArray(int* arr) {
  for (int i = 0; i < N; i++) {
    printf("%3d ", arr[i]);
  }
  printf("\n");
}

int** GetTriangleMatrix(int** matrix) {
  int c = N - 1;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (j >= c) {
        matrix[i][j] = 1;
      } else {
        matrix[i][j] = 0;
      }
    }
    c--;
  }
  return matrix;
}

int** GetSpiralMatrix(int** matrix) {
  int dir = 1, step = N, c = 0, cycle = 0, i = 0;
  do {
    switch (dir) {
      case 1:
        for (int k = cycle; k < step; k++) {
          c++;
          matrix[i][k] = c;
        }
        i = N - cycle - 1;
        dir++;
        step--;
        break;
      case 2:
        for (int k = cycle + 1; k < step; k++) {
          c++;
          matrix[k][i] = c;
        }
        dir++;
        break;
      case 3:
        for (int k = N - cycle - 1; k >= cycle; k--) {
          c++;
          matrix[i][k] = c;
        }
        dir++, i = cycle;
        break;
      case 4:
        for (int k = N - cycle - 2; k > cycle; k--) {
          c++;
          matrix[k][i] = c;
        }
        dir = 1;
        cycle++;
        i = cycle;
        break;
    }
  } while (c < N * N);
  return matrix;
}

int main() {
  printf("1. Вывести квадратную матрицу по заданному N. \n");
  int** matrix = CreateMatrix();
  PrintMatrix(matrix);

  printf("\n2. Вывести заданный массив размером N в обратном порядке. \n");
  int* arr = CreateArray();
  PrintArray(arr);
  arr = ReverseArray(arr);
  PrintArray(arr);

  printf("\n3. Заполнить верхний треугольник матрицы 0, а нижний 1. \n");
  matrix = GetTriangleMatrix(matrix);
  PrintMatrix(matrix);

  printf("\n4. Заполнить матрицу числами от 1 до N2 улиткой.\n");
  matrix = GetSpiralMatrix(matrix);
  PrintMatrix(matrix);

  free(arr);
  FreeMatrix(matrix);
  return 0;
}