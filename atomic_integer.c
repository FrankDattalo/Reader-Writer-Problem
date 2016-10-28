#ifndef ATOMIC_INTEGER_C
#define ATOMIC_INTEGER_C

#include <pthread.h>

typedef struct atomic_integer {
  int value;
  pthread_mutex_t mutex;
} atomic_integer;

void atomic_init(atomic_integer* item) {
  item->value = 0;
  pthread_mutex_init(&item->mutex, NULL);
}

void atomic_init_and_set(atomic_integer* item, int value) {
  item->value = value;
  pthread_mutex_init(&item->mutex, NULL);
}

void atomic_set(atomic_integer* item, int value) {
  pthread_mutex_lock(&item->mutex);
  item->value = value;
  pthread_mutex_unlock(&item->mutex);
}

int atomic_get(atomic_integer* item) {
  pthread_mutex_lock(&item->mutex);
  int value = item->value;
  pthread_mutex_unlock(&item->mutex);
  return value;
}

void atomic_increment(atomic_integer* item) {
  pthread_mutex_lock(&item->mutex);
  item->value++;
  pthread_mutex_unlock(&item->mutex);
}

int atomic_increment_and_get(atomic_integer* item) {
  pthread_mutex_lock(&item->mutex);
  item->value++;
  int value = item->value;
  pthread_mutex_unlock(&item->mutex);
  return value;
}

int atomic_decrement_and_get(atomic_integer* item) {
  pthread_mutex_lock(&item->mutex);
  item->value--;
  int value = item->value;
  pthread_mutex_unlock(&item->mutex);
  return value;
}

void atomic_decrement(atomic_integer* item) {
  pthread_mutex_lock(&item->mutex);
  item->value--;
  pthread_mutex_unlock(&item->mutex);
}

#endif /* ATOMIC_INTEGER_C */
