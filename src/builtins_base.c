#include "lisp.h"
#include "builtins_base.h"

#include <stdio.h>

DEFUN("<", lisp_lt, VAR_FIXED, 2) {
  struct lisp_object *first = HEAD(args);
  struct lisp_object *second = first->next;

  if (first->obj_type != INTEGER || second->obj_type != INTEGER) {
    /* We don't know if this is correct */
  }

  if (TOLINT(first) < TOLINT(second)) {
    return t;
  }
  else {
    return nil;
  }
}

DEFUN(">", lisp_gt, VAR_FIXED, 2) {
  struct lisp_object *first = HEAD(args);
  struct lisp_object *second = first->next;

  if (first->obj_type != INTEGER || second->obj_type != INTEGER) {
    /* Signal an error */
  }

  if (TOLINT(first) > TOLINT(second)) {
    return t;
  }
  else {
    return nil;
  }
}

DEFUN("=", lisp_eq, VAR_FIXED, 2) {
  struct lisp_object *first = HEAD(args);
  struct lisp_object *second = first->next;

  if (first->obj_type != INTEGER || second->obj_type != INTEGER) {
    /* Signal an error */
  }

  if (TOLINT(first) == TOLINT(second)) {
    return t;
  }
  else {
    return nil;
  }
}

/*
 * (IF CONDITION THEN-CLAUSE ELSE-CLAUSE)
 */
DEFUN("if", lisp_if, VAR_MIN | UNEVAL_ARGS, 2) {
  int length = list_length(args);

  if (length > 3) {
    /* Signal an error of some kind */
  }

  struct lisp_object *head = HEAD(args);

  struct lisp_object *condition = c_eval(head);
  struct lisp_object *then_clause = head->next;
  struct lisp_object *else_clause = NULL;
  if (length == 3) {
    else_clause = then_clause->next;
  }

  if (TRUEP(condition)) {
    return c_eval(then_clause);
  }
  else if (else_clause) {
    return c_eval(else_clause);
  }
  else {
    /* Don't execute anything because we weren't given a parameter for 'else' */
    return nil;
  }
}

DEFUN("prints", lisp_prints, VAR_FIXED, 2) {
  char *to_print = TOSTR(HEAD(args));

  printf("%s\n", to_print);

  return nil;
}

DEFUN("print", lisp_print, VAR_FIXED, 2) {
  c_print(HEAD(args));

  return nil;
}

void base_initialize() {
  lisp_if_init();
  lisp_lt_init();
  lisp_gt_init();
  lisp_eq_init();
  lisp_print_init();
  lisp_prints_init();
}
