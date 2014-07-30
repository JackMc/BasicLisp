#include "lisp.h"

#include <stdio.h>
#include <errno.h>

int main(int argc, char **argv) {
  FILE *read_from;
  
  switch (argc) {
  case 1:
    read_from = NULL;
    break;
  case 2: {
    read_from = fopen(argv[1], "r");
    if (!read_from) {
      perror("Error while opening main file: ");
    }
    break;
  }
  default:
    printf("Too many arguments. Use like %s [file]", argv[0]);
    break;
  }
  
  lisp_initialize();
  while (C_TRUE) {
    printf(">>> ");
    c_print(c_eval(c_read(read_from)));
  }
}
