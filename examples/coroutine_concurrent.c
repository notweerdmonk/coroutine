#include <stdio.h>
#include <coroutine.h>
#include <pthread.h>

void* delegated(int m, int tid) {
  
  /* Solve concurrent access problem by using thread local storage */
  static thread_local int n;

  COROUTINE_BEGIN();

  printf("delegated() called 1st time, tid = %d, m = %d, n = %d.\n", tid, m,
      ++n);

  COROUTINE_YIELD();

  printf("delegated() called 2nd time, tid = %d, m = %d, n = %d.\n", tid, m,
      ++n);

  COROUTINE_YIELD();

  printf("delegated() called 3rd time, tid = %d, m = %d, n = %d.\n", tid, m,
      ++n);

  COROUTINE_STOP();
}

void* coroutine(int tid) {

  /* Solve concurrent access problem by using thread local storage */
  static thread_local int m;

  COROUTINE_BEGIN();

  printf("Start of coroutine(), tid = %d,  m = %d.\n", tid, ++m);

  COROUTINE_YIELD();

  printf("Delegating coroutine, tid = %d, m = %d.\n", tid, m);

  COROUTINE_DELEGATE(delegated, m, tid);

  printf("Calling coroutine() after delegated() finishes, tid = %d, m = %d.\n",
      tid, ++m);

  COROUTINE_END();
}

void *thread_fn(void *arg) {
  int thread_id = *(int*)arg;

  for (int i = 0; i < 4; i++) {
    coroutine(thread_id);
  }

  pthread_exit(NULL);
}

int main() {

  pthread_t thread_id;
  int id = 1;

  if (pthread_create(&thread_id, NULL, thread_fn, &id) != 0) {
    fprintf(stderr, "Error creating thread\n");
    return 1;
  }

  for (int i = 0; i < 4; i++) {
    coroutine(0);
  }

  pthread_join(thread_id, NULL);

  return 0;
}
