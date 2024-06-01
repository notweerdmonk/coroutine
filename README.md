# coroutine

###### coroutines in C

Macros that implement coroutines using Duff's device[^1] based on Simon Tatham's writeup[^2].

Take a look at this function for evaluating postfix expressions containing non-negative integers. Portions that handle errors have been omitted. Normally such a function would be fed an array of characters which will be iterated over, and the function shall return the result of the expression. A different approach is to call the function on each character as soon as it is available. Some internal state needs to be maintained by the function across its calls. Coroutines do just that.

In the source code below, `COROUTINE_YIELD()` causes the function to return and execution shall resume from this point upon the next call. It is quite evident where to use this macro, at the end of the block that processes each character. Yet another place will be after the processing of each character consisting a positive integer.

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


[^1]: https://en.wikipedia.org/wiki/Duff's_device
[^2]: https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html
