#include <stdio.h>
#include <ctype.h> /* for isdigit */
#include <limits.h> /* for INT_MAX */
#include <coroutine.h>

#define STACK_MAX_SIZE 100

struct stack {
  int top;
  int items[STACK_MAX_SIZE];
};

void stack_init(struct stack *s) {
  s->top = -1;
}

int stack_is_full(struct stack *s) {
  return s->top == STACK_MAX_SIZE - 1;
}

int stack_is_empty(struct stack *s) {
  return s->top == -1;
}

int stack_size(struct stack *s) {
  return s->top + 1;
}

void stack_push(struct stack *s, int value) {
  if (stack_is_full(s)) {
    return;
  }

  s->items[++s->top] = value;
}

int stack_pop(struct stack *s) {
  if (stack_is_empty(s)) {
    return INT_MAX;
  }

  return s->items[s->top--];
}

int isoperator(char c) {
  char op[] = { '+', '-', '*', '/' };

  for (size_t i = 0; i < sizeof(op)/sizeof(char); i++) {
    if (op[i] == c) {
      return 1;
    }
  }

  return 0;
}

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
      if (stack_size(&s) < 2) {
        printf("Not enough operands\n");
        COROUTINE_ERROR();
      }

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
          if (operand2 == 0) {
            printf("Division by zero\n");
            COROUTINE_ERROR();
          }

          stack_push(&s, operand1 / operand2);
          break;
        default:
          printf("Invalid operator\n");
          COROUTINE_ERROR();
      }

    } else if (expr == '\0') {
      if (stack_size(&s) != 1) {
        printf("Malformed expression\n");
        COROUTINE_ERROR();
      }

      if (!result) {
        printf("Null pointer argument\n");
        COROUTINE_ERROR();
      }
      *result = stack_pop(&s);

      break;

    } else if (expr != ' ') {
      printf("Not a digit or operator\n");
      COROUTINE_ERROR();
    }

    COROUTINE_YIELD();
  }

  COROUTINE_END();
}

int main(int argc, char *argv[]) {

  int result;
  char c = 0;

  if (argc > 1) {
    printf("Command line postfix expression: ");

    for (int i = 1; i < argc; i++) {
      for (char *p = argv[i]; *p; p++) {
        if (!evaluate_postfix(*p, &result)) {
          goto next;
        }
        putchar(*p);
      }

      evaluate_postfix(' ', &result);
      putchar(' ');
    }
    if (!evaluate_postfix('\0', &result)) {
      goto next;
    }
    putchar('\n');

    printf("Result: %d\n", result);
  }

next:
  printf("Enter (x) to exit\n");

  for(;;) {
end:
    printf("Enter postfix expression: ");

    while ( (c = getchar()) ) {
      /* exit if x is entered */
      if (c == 'x') {
        return 0;

      } else if (c == '\n' || c == '\r') {
        c = '\0';
        break;
      }

      if (!evaluate_postfix(c, &result)) {
        goto end;
      }
    }
    if (!evaluate_postfix(c, &result)) {
      continue;
    }

    printf("Result: %d\n", result);
  }
      
  return 0;
}
