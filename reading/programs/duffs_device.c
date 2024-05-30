/* https://en.wikipedia.org/wiki/Duff's_device */

#include <stdio.h>
#include <string.h>
#include <assert.h>

void send(short *to, short *from, char count) {
  do { /* count > 0 assumed */
    *to++ = *from++;
  } while (--count > 0);
}

/* if count is always divisible by 8 */
void send_unroll(short *to, short *from, char count) {
  char n = count / 8;
  do {
    *to++ = *from++;
    *to++ = *from++;
    *to++ = *from++;
    *to++ = *from++;
    *to++ = *from++;
    *to++ = *from++;
    *to++ = *from++;
    *to++ = *from++;
  } while (--n > 0);
}

/* using Duff's device to handle cases where count is not divisible by eight */
void send_duff(short *to, short *from, char count) {
  char n = (count + 7) / 8;
  switch (count % 8) {
  case 0: do { *to++ = *from++;
  case 7:      *to++ = *from++;
  case 6:      *to++ = *from++;
  case 5:      *to++ = *from++;
  case 4:      *to++ = *from++;
  case 3:      *to++ = *from++;
  case 2:      *to++ = *from++;
  case 1:      *to++ = *from++;
          } while (--n > 0);
  }
}

void print_array(short *arr, char size) {
  for (int i = 0; i < size; i++) {
    printf("0x%02x ", arr[i]);
  }
  printf("\n");
}

int main() {

  short src[] = { 0x7000, 0x7111, 0x7222, 0x7333,
                  0x7444, 0x7555, 0x7666, 0x7777,
                  0x7888, 0x7999, 0x7aaa, 0x7bbb,
                  0x7ccc, 0x7ddd, 0x7eee, 0x7ead };

  short dst[16];

  memset(dst, 0, sizeof(dst));
  printf("Normal send\n");
  send(dst, src, 10);
  assert(memcmp(dst, src, 10 * sizeof(short)) == 0 && "Normal send mismatch");
  print_array(dst, 10);

  memset(dst, 0, sizeof(dst));
  printf("Loop unrolled\n");
  send_unroll(dst, src, 16);
  assert(memcmp(dst, src, 16 * sizeof(short)) == 0 && "Loop unrolled mismatch");
  print_array(dst, 16);

  memset(dst, 0, sizeof(dst));
  printf("Duff's device\n");
  send_duff(dst, src, 10);
  assert(memcmp(dst, src, 10 * sizeof(short)) == 0 && "Duff's device mismatch");
  print_array(dst, 10);

  return 0;
}
