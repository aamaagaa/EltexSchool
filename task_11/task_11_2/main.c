#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREADS_NUM 5
#define BIG_NUM 10000000
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;

long result;

void* thread_calc(){
  for(long i = 0; i < BIG_NUM/THREADS_NUM; i++){
    pthread_mutex_lock(&m1);
    result++;
    pthread_mutex_unlock(&m1);
  }
  return NULL;
}

int main(){
  pthread_t threads[THREADS_NUM];

  for(int i = 0; i < THREADS_NUM; i++){
    pthread_create(&threads[i], NULL, thread_calc, NULL);
  }

  for(int j = 0; j < THREADS_NUM; j++){
    pthread_join(threads[j], NULL);
  }

  printf("result = %ld\n", result);

  return EXIT_SUCCESS;
}