#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <coroutine.h>

void* generator(int start, int stop, int step, int *val) {

  static thread_local int *range = NULL;
  size_t len = (stop - start) / (step == 0 ? 1 : step); 

  COROUTINE_BEGIN();

  if (!range) {
    if(!(range = (int*)malloc(len * sizeof(int)))) {
#ifdef __AVR
      printf("Malloc for size %u failed\n", len * sizeof(int));
#else
      printf("Malloc for size %ld failed: %s\n", len * sizeof(int),
          strerror(errno));
#endif
      COROUTINE_ERROR();
    }

    for (size_t i = 0; i < len; i++) {
      range[i] = start + (i * step);
    }
  }

  static thread_local size_t idx = 0;
  do {
    if (val) {
      *val = range[idx];
    }

    if (++idx == len) {
      break;
    }
    COROUTINE_YIELD();

  } while (idx < len);

  idx = 0;
  free(range);
  range = NULL;

  COROUTINE_RESET();
  COROUTINE_END();
}

int main() {

  for (int i = 0; i < 10; i++) {
    int val;
    if (i == 2) {
      generator(0, 10, 2, NULL);
    } else {
      generator(0, 10, 2, &val);
    }
    printf("Count is %d\n", val);
  }

  return 0;
}
