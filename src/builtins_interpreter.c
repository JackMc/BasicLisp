/*
 * Defines a set of functions which are only initialized when we are using the interactive interpreter
 */

#include "lisp.h"

#include <stdlib.h>

DEFUN("exit", lisp_exit, VAR_FIXED, 0) {
  exit(0);
}

void interpreter_initialize() {
  lisp_exit_init();
}
