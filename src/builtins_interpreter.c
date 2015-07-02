/*
 * Defines a set of functions which are only initialized when we are using the interactive interpreter
 */

#include "lisp.h"

#include <stdlib.h>

DEFUN("exit", lisp_exit, EVAL_ARGS, 0, 1) {
  exit(args->next ? (int)TOLINT(args->next) : 0);
}

void interpreter_initialize() {
  lisp_exit_init();
}
