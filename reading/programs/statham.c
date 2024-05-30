/* https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_TOKEN_LEN 16

enum token_type {
  WORD,
  PUNCT
};

struct token {
  unsigned char tok[MAX_TOKEN_LEN];
  int len;
  enum token_type type;
} *ptoken;

int tokidx;

struct node {
  struct node *prev, *next;
  struct token *key;
} *head, *tail;

void add_to_token(int c) {
  if (!ptoken) {
    if ( !( ptoken = (struct token *)malloc(sizeof(struct token)) ) ) {
      return;
    }
  }

  if (tokidx >= MAX_TOKEN_LEN) {
    return;
  }

  ptoken->tok[tokidx++] = c;
}

void got_token(enum token_type type) {
  if (!ptoken) {
    return;
  }

  ptoken->type = type;
  ptoken->len = tokidx;
  tokidx = 0;

  if (!head) {
    if ( !( head = (struct node *)malloc(sizeof(struct node)) ) ) {
      return;
    }

    head->prev = head->next = NULL;

    tail = head;

  } else {
    struct node *new;
    if ( !( new = (struct node *)malloc(sizeof(struct node)) ) ) {
      return;
    }

    new->next = NULL;
    new->prev = tail;
    tail->next = new;

    tail = new;
  }

  tail->key = ptoken;

  ptoken = NULL;
}

void print_token_list(struct node *head) {
  while (head) {
    if (!head->key) {
      continue;
    }

    struct token *p = head->key;
    printf("Token type: %d Token[0]: %02x Token: %-*s\n", p->type, p->tok[0], p->len, p->tok);

    head = head->next;
  }
}

void destroy_token(struct token *ptoken) {
  if (!ptoken) {
    return;
  }

  free(ptoken);
}

void destroy_token_list(struct node *head) {
  while (head) {
    struct node *tmp = head;
    head = head->next;

    destroy_token(tmp->key);
    free(tmp);
  }

  tail = head;
}

/*

// Decompression code
while (1) {
  c = getchar();
  if (c == EOF)
    break;
  if (c == 0xFF) {
    len = getchar();
    c = getchar();
    while (len--)
      emit(c);
  } else
    emit(c);
}
emit(EOF);

// Parser code
while (1) {
  c = getchar();
  if (c == EOF)
    break;
  if (isalpha(c)) {
    do {
      add_to_token(c);
      c = getchar();
    } while (isalpha(c));
    got_token(WORD);
  }
  add_to_token(c);
  got_token(PUNCT);
}

*/

int decompressor(void) {
  static int repchar;
  static int replen;
  if (replen > 0) {
    replen--;
    return repchar;
  }
  int c = getchar();
  if (c == EOF)
    return EOF;
  if (c == 0xFF) {
    replen = getchar();
    repchar = getchar();
    replen--;
    return repchar;
  } else
    return c;
}

void parser(int c) {
  static enum {
    START, IN_WORD
  } state;
  switch (state) {
    case IN_WORD:
      if (isalpha(c)) {
        add_to_token(c);
        return;
      }
      got_token(WORD);
      state = START;
      /* fall through */

    case START:
      add_to_token(c);
      if (isalpha(c))
        state = IN_WORD;
      else
        got_token(PUNCT);
      break;
  }
}

int main() {

#if 0
  while (1) {
    int c = getchar();
    if (c == EOF)
      break;
    if (c == 0xFF) {
      int len = getchar();
      c = getchar();
      while (len--)
        parser(c);
    } else
      parser(c);
  }
#else
  while (1) {
    int c = decompressor();
    if (c == EOF)
      break;
    if (isalpha(c)) {
      do {
        add_to_token(c);
        c = decompressor();
      } while (isalpha(c));
      got_token(WORD);
    }
    add_to_token(c);
    got_token(PUNCT);
  }
#endif

  print_token_list(head);

  destroy_token_list(head);

  return 0;
}
