#include "lisp.h"

int main(int argc, char **argv) {
  lisp_initialize();
  while (C_TRUE) {
    printf(">>> ");
    c_print(c_eval(c_read(NULL)));
  }
}
