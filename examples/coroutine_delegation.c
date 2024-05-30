#include <stdio.h>
#include <coroutine.h>

void* delegated(int m) {
  
  static int n;

  COROUTINE_BEGIN();

  printf("delegated() called 1st time, m = %d, n = %d.\n", m, ++n);

  COROUTINE_YIELD();

  printf("delegated() called 2nd time, m = %d, n = %d.\n", m, ++n);

  COROUTINE_YIELD();

  printf("delegated() called 3rd time, m = %d, n = %d.\n", m, ++n);

  COROUTINE_STOP();
}

void* coroutine() {

  static int m;

  COROUTINE_BEGIN();

  printf("Start of coroutine(), m = %d.\n", ++m);

  COROUTINE_YIELD();

  printf("Delegating coroutine, m = %d.\n", m);

  COROUTINE_DELEGATE(delegated, m);

  printf("Calling coroutine() after delegated() finishes, m = %d.\n", ++m);

  COROUTINE_END();
}

int main() {

  for (int i = 0; i < 8; i++) {
    coroutine();
  }

  return 0;
}
