#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SHOPS_NUM 5
#define CUSTOMERS_NUM 3
#define THREADS_NUM 4
#define CARGO 5000
#define NEED 100000
#define SLEEP 2
#define SLEEP_CARGO 1
#define MAX 10000

struct Shop {
  pthread_mutex_t m;
  int products;
};

struct Shop Shops[SHOPS_NUM];
int Customers[CUSTOMERS_NUM];
pthread_t Threads[THREADS_NUM];
pthread_mutex_t customers_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_shops() {
  srand(time(NULL));
  for (int i = 0; i < SHOPS_NUM; i++) {
    if (pthread_mutex_init(&Shops[i].m, NULL) != 0) {
      perror("pthread_mutex_init");
      exit(EXIT_FAILURE);
    }
    Shops[i].products = MAX / 2 + rand() % (MAX - MAX / 2 + 1);
  }
}

void init_customers() {
  for (int i = 0; i < CUSTOMERS_NUM; i++) {
    Customers[i] = NEED / 2 + rand() % (NEED - NEED / 2 + 1);
  }
}

void* customer(void* customer_idx) {
  int idx = *(int*)customer_idx;
  do {
    for (int i = 0; i < SHOPS_NUM; i++) {
      if (pthread_mutex_trylock(&Shops[i].m) == 0) {
        if (Customers[idx] > Shops[i].products) {
          printf("Покупатель '%d' купил %-6d товаров в магазине '%d'. Потребность: %-6d. В магазине пусто!\n", idx,
                 Shops[i].products, i, Customers[idx] - Shops[i].products);
          Customers[idx] -= Shops[i].products;
          Shops[i].products = 0;
        } else {
          printf("Покупатель '%d' купил %-6d товаров в магазине '%d'. Ему больше не надо! Товаров в магазине: %-6d\n", idx,
                 Customers[idx], i, Shops[i].products - Customers[idx]);
          Shops[i].products -= Customers[idx];
          Customers[idx] = 0;
          pthread_mutex_unlock(&Shops[i].m);
          break;
        }
        pthread_mutex_unlock(&Shops[i].m);
        sleep(SLEEP);
      }
    }
  } while (Customers[idx] > 0);
  return NULL;
}

void create_shops(){
  int *idx = malloc(CUSTOMERS_NUM * sizeof(int));
  for (int i = 0; i < CUSTOMERS_NUM; i++) {
    idx[i] = i;
    pthread_create(&Threads[i], NULL, customer, (void*)&idx[i]);
  }
}

int need_to_continue(){
  pthread_mutex_lock(&customers_mutex);
  for (int i = 0; i < CUSTOMERS_NUM; i++) {
    if(Customers[i] > 0){
      pthread_mutex_unlock(&customers_mutex); 
      return 1;
    }
  }
  pthread_mutex_unlock(&customers_mutex);
  return 0;
}

void* truck() {
  do {
    for (int i = 0; i < SHOPS_NUM; i++) {
      if (pthread_mutex_trylock(&Shops[i].m) == 0) {
        printf("Грузовик привез товары в магазин '%d'\n", i);
        Shops[i].products += CARGO;
        pthread_mutex_unlock(&Shops[i].m);
        sleep(SLEEP_CARGO);
      }
    }
  } while (need_to_continue());
  return NULL;
}

void create_truck(){
  pthread_create(&Threads[CUSTOMERS_NUM], NULL, truck, NULL);
}

void print_info() {
  for (int i = 0; i < SHOPS_NUM; i++) {
    printf("В магазине '%d' - %d товаров\n", i, Shops[i].products);
  }
  for (int i = 0; i < CUSTOMERS_NUM; i++) {
    printf("Покупателю '%d' нужно %d товаров\n", i, Customers[i]);
  }
}

void clean_mutex(){
  for (int i = 0; i < SHOPS_NUM; i++) {
    pthread_mutex_destroy(&Shops[i].m);
  }
  pthread_mutex_destroy(&customers_mutex);
}

void wait_for_threads(){
  for (int j = 0; j < THREADS_NUM; j++) {
    pthread_join(Threads[j], NULL);
  }
}

int main() {
  init_shops();
  init_customers();
  print_info();
  create_shops();
  create_truck();
  wait_for_threads();
  print_info();
  clean_mutex();
  return EXIT_SUCCESS;
}
