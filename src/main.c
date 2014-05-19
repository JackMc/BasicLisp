#include "lisp.h"

int main(int argc, char **argv) {
  lisp_initialize();
  c_print(c_eval(c_read(NULL)));
}
