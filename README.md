# coroutine

###### coroutines in C

Macros that implement coroutines using Duff's device[^1] based on Simon Tatham's writeup[^2].

Take a look at this function for evaluating postfix expressions containing non-negative integers. Portions that handle errors have been omitted. Normally such a function would be fed an array of characters which will be iterated over, and the function shall return the result of the expression. A different approach is to call the function on each character as soon as it is available. Some internal state needs to be maintained by the function across its calls. Coroutines do just that.

In the source code below, `COROUTINE_YIELD()` causes the function to return and execution shall resume from this point upon the next call. It is quite evident where to use this macro, at the end of the block that processes each character. Another place will be after the processing of each character consisting a positive integer. Coroutines should be enclosed within `COROUTINE_BEGIN()` and `COROUTINE_END()`. At the moment there can be only one coroutine inside a function.

```c
void* evaluate_postfix(char expr, int *result) {

  COROUTINE_BEGIN();

  static struct stack s;
  stack_init(&s);

  while (1) {
    if (isdigit(expr)) {
      static int operand = 0;

      do {
        operand = operand * 10 + (expr - 48);
        COROUTINE_YIELD();
      } while (isdigit(expr)) ;
    
      stack_push(&s, operand);
      operand = 0;
    }
    
    if (isoperator(expr)) {
    
      int operand2 = stack_pop(&s);
      int operand1 = stack_pop(&s);
      switch (expr) {
        case '+':
          stack_push(&s, operand1 + operand2);
          break;
        case '-':
          stack_push(&s, operand1 - operand2);
          break;
        case '*':
          stack_push(&s, operand1 * operand2);
          break;
        case '/':
    
          stack_push(&s, operand1 / operand2);
          break;
        default:
          ;
      }
    
    } else if (expr == '\0') {
      *result = stack_pop(&s);
      break;
    
    }
    
    COROUTINE_YIELD();
  }

  COROUTINE_END();
}
```

#### Concurrency

Concurrently calling a coroutine function from multiple threads is addressed by placing coroutine state variables in thread local storage[^3]. Local variables in such coroutine functions can also be placed in TLS.

```c
void* coroutine(void) {
  /* Solve concurrent access problem by using thread local storage */
  static thread_local int n;

  COROUTINE_BEGIN();
  COROUTINE_YIELD();
  COROUTINE_END();
}
```

However TLS can be restricted in size. A block memory can be dynamically allocated on the heap and used to store all internal variables of the coroutine function. The address of this block can be stored in the TLS.

```c
struct coroutine_data {
  int n;
};

void* coroutine(void) {
  /* Allocate memory dynamically and store its address in the TLS */
  static thread_local struct coroutine_data *ptr =
    (struct coroutine_data*)malloc(sizeof(struct coroutine_data));
    
  COROUTINE_BEGIN();
  COROUTINE_YIELD();
  COROUTINE_END();
}
```

As opposed to using the heap along with TLS, the coroutine function specific data can be placed on stack as local variables for the caller and passed to the coroutine function as arguments.

```c
struct coroutine_data {
  long l;
};

void* coroutine(struct coroutine_data *pdata) {
  COROUTINE_BEGIN();
  // use pdata->l
  COROUTINE_YIELD();
  // use pdata->l
  COROUTINE_END();
}

void thread_fn(void) {
  struct coroutine_data data = { .l = 80 };
  for (int i = 0; i < 3; i++) {
    coroutine(&data);
  }
}

int main(void) {
  pthread_t thread_id;
  struct coroutine_data data = { .l = 0x73616a6e61 };
    
  pthread_create(&thread_id, NULL, thread_fn, NULL);

  for (int i = 0; i < 3; i++) {
    coroutine(&data);
  }

  pthread_join(thread_id, NULL);
  return 0;
}
```

#### Nested coroutines

Calls to coroutine functions can be nested. Execution is delegated to a child coroutine function using `COROUTINE_DELEGATE(delegated_fn, ...)`. Each time the parent function gets called it shall in turn call the child function until the child function uses `COROUTINE_RESET()` or `COROUTINE_STOP()` (instead of `COROUTINE_END()`). After the child coroutine terminates, execution shall continue from the point at which `COROUTINE_DELEGATE` got used in the parent coroutine function. The example below demonstrates nested coroutines with a depth of two.

```c
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
```

Above source code produces the following output.

```shell
Start of coroutine(), m = 1.
Delegating coroutine, m = 1.
delegated() called 1st time, m = 1, n = 1.
delegated() called 2nd time, m = 1, n = 2.
delegated() called 3rd time, m = 1, n = 3.
Calling coroutine() after delegated() finishes, m = 2.
Start of coroutine(), m = 3.
Delegating coroutine, m = 3.
delegated() called 1st time, m = 3, n = 4.
delegated() called 2nd time, m = 3, n = 5.
delegated() called 3rd time, m = 3, n = 6.
Calling coroutine() after delegated() finishes, m = 4.
```



[^1]: https://en.wikipedia.org/wiki/Duff's_device
[^2]: https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html
[^3]: https://en.wikipedia.org/wiki/Thread-local_storage

