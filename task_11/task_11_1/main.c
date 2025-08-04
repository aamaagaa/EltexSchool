#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREADS_NUM 5

void* print_idx(void* thread_num){
  printf("thread idx: %d\n", *(int*)thread_num);
  return NULL;
}

int main(){
  pthread_t threads[THREADS_NUM];
  int idx[THREADS_NUM];

  for(int i = 0; i < THREADS_NUM; i++){
    idx[i] = i;
    pthread_create(&threads[i], NULL, print_idx, (void*)&idx[i]);
  }

  for(int j = 0; j < THREADS_NUM; j++){
    pthread_join(threads[j], NULL);
  }

  return EXIT_SUCCESS;
}