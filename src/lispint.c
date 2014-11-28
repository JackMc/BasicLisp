#include "lispint.h"

static int num_long_digits;

static void determine_long_digits() {
  num_long_digits = 0;
  long n = LONG_MAX;

  while (n) {
    num_long_digits++;
    n /= 10;
  }
}

struct lisp_int *lisp_int_initializef(FILE *f) {
  fpos_t pos;
  char c = 0;
  int digits = 0;

  // Determine the length of the int in digits
  fgetpos(f, &pos);
  while(isdigit(fgetc(f)) && ++digits);
  fsetpos(f, &pos);

  if (!num_long_digits) {
    determine_long_digits();
  }

  // We only use the space up to long digits - 1 because we can work easier
  // in a system with a power of ten as the max rather than a power of 2
  if (digits < (num_long_digits - 1)) {
    // We can just store it in a single long! :D

  }
}
