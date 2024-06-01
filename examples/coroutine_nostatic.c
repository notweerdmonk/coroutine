#include <stdio.h>
#include <coroutine.h>
#include <pthread.h>

struct delegated_data {
  int tid;
  int m;
  int n;
};
    
void* delegated(struct delegated_data *pdata) {
  
  COROUTINE_BEGIN();

  printf("delegated() called 1st time, tid = %d, m = %d, n = %d.\n",
      pdata->tid, pdata->m, ++(pdata->n));

  COROUTINE_YIELD();

  printf("delegated() called 2nd time, tid = %d, m = %d, n = %d.\n",
      pdata->tid, pdata->m, ++(pdata->n));

  COROUTINE_YIELD();

  printf("delegated() called 3rd time, tid = %d, m = %d, n = %d.\n",
      pdata->tid, pdata->m, ++(pdata->n));

  COROUTINE_STOP();
}

struct coroutine_data {
  int tid;
  int m;
};

void* coroutine(struct coroutine_data *pdata) {

  COROUTINE_BEGIN();

  printf("Start of coroutine(), tid = %d,  m = %d.\n", pdata->tid,
      ++(pdata->m));

  COROUTINE_YIELD();

  printf("Delegating coroutine, tid = %d, m = %d.\n", pdata->tid, pdata->m);

  struct delegated_data data = { pdata->tid, pdata->m, 0 };

  COROUTINE_DELEGATE(delegated, &data);

  printf("Calling coroutine() after delegated() finishes, tid = %d, m = %d.\n",
      pdata->tid, ++(pdata->m));

  COROUTINE_END();
}

void *thread_fn(void *arg) {
  int thread_id = *(int*)arg;

  struct coroutine_data data = { thread_id, 0 };

  for (int i = 0; i < 4; i++) {
    coroutine(&data);
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

  struct coroutine_data data = { 0, 0 };

  for (int i = 0; i < 4; i++) {
    coroutine(&data);
  }

  pthread_join(thread_id, NULL);

  return 0;
}
