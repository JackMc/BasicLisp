#ifndef LISP_INT_H_
#define LISP_INT_H_

#include <stdio.h>

// Measured in units of LONG_MAX
#define DEFAULT_PRECISION 3

struct lisp_int {
  long *rep;
  // Measured in units of LONG_MAX. Default DEFAULT_PRECISION
  int rep_len;
};

struct lisp_int *lisp_int_initializef(FILE *f);

struct lisp_int *lisp_int_initializes(char *s);

char *lisp_int_to_str(struct lisp_int *i);

#endif // LISP_INT_H_
