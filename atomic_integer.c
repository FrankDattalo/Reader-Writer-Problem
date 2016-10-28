#ifndef ATOMIC_INTEGER_C
#define ATOMIC_INTEGER_C

#include <pthread.h>

/*
  This file contains an atomic_integer struct as well as the operations that
  can be performed on the struct.  atomic_integer ensures atomicity over it's
  operations and thus can be used in a thread safe way.
*/

typedef struct atomic_integer {
  int value;
  pthread_mutex_t mutex;
} atomic_integer;

/* int item = 0; */
void atomic_init(atomic_integer* item) {
  item->value = 0;
  pthread_mutex_init(&item->mutex, NULL);
}

/* int item = value; */
void atomic_init_and_set(atomic_integer* item, int value) {
  item->value = value;
  pthread_mutex_init(&item->mutex, NULL);
}

/* item = value; */
void atomic_set(atomic_integer* item, int value) {
  pthread_mutex_lock(&item->mutex);
  item->value = value;
  pthread_mutex_unlock(&item->mutex);
}

/* return item; */
int atomic_get(atomic_integer* item) {
  pthread_mutex_lock(&item->mutex);
  int value = item->value;
  pthread_mutex_unlock(&item->mutex);
  return value;
}

/* item++; */
void atomic_increment(atomic_integer* item) {
  pthread_mutex_lock(&item->mutex);
  item->value++;
  pthread_mutex_unlock(&item->mutex);
}

/* return ++item; */
int atomic_increment_and_get(atomic_integer* item) {
  pthread_mutex_lock(&item->mutex);
  item->value++;
  int value = item->value;
  pthread_mutex_unlock(&item->mutex);
  return value;
}

/* return --item; */
int atomic_decrement_and_get(atomic_integer* item) {
  pthread_mutex_lock(&item->mutex);
  item->value--;
  int value = item->value;
  pthread_mutex_unlock(&item->mutex);
  return value;
}

/* item--; */
void atomic_decrement(atomic_integer* item) {
  pthread_mutex_lock(&item->mutex);
  item->value--;
  pthread_mutex_unlock(&item->mutex);
}

#endif /* ATOMIC_INTEGER_C */
