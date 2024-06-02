#include <stdio.h>
#include <coroutine.h>

void *delegated2(int m, int n) {
  static int o;

  COROUTINE_BEGIN()

  printf("delegated2() called 1st time, m = %d, n = %d, o = %d.\n", m, n, ++o);

  COROUTINE_YIELD();

  printf("delegated2() called 2nd time, m = %d, n = %d, o = %d.\n", m, n, ++o);

  COROUTINE_YIELD();

  printf("delegated2() called 2nd time, m = %d, n = %d, o = %d.\n", m, n, ++o);

  COROUTINE_STOP();
}

void* delegated1(int m) {
  
  static int n;

  COROUTINE_BEGIN();

  printf("delegated1() called 1st time, m = %d, n = %d.\n", m, ++n);

  COROUTINE_YIELD();

  printf("delegated1() called 2nd time, m = %d, n = %d.\n", m, ++n);

  COROUTINE_YIELD();

  printf("delegated1() called 3rd time, m = %d, n = %d.\n", m, ++n);

  COROUTINE_DELEGATE(delegated2, m, n);

  COROUTINE_STOP();
}

void* coroutine() {

  static int m;

  COROUTINE_BEGIN();

  printf("Start of coroutine(), m = %d.\n", ++m);

  COROUTINE_YIELD();

  printf("Delegating coroutine, m = %d.\n", m);

  COROUTINE_DELEGATE(delegated1, m);

  printf("Calling coroutine() after delegated1() finishes, m = %d.\n", ++m);

  COROUTINE_END();
}

int main() {

  for (int i = 0; i < 12; i++) {
    coroutine();
  }

  return 0;
}
