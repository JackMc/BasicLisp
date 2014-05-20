#include "lisp.h"
#include "builtins_base.h"

#include <stdio.h>
#include <stdlib.h>

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

DEFUN("prints", lisp_prints, VAR_FIXED, 1) {
  char *to_print = TOSTR(HEAD(args));

  printf("%s\n", to_print);

  return nil;
}

DEFUN("print", lisp_print, VAR_FIXED, 1) {
  c_print(HEAD(args));

  return nil;
}

DEFUN("setq", lisp_setq, VAR_FIXED | UNEVAL_ARGS, 2) {
  struct lisp_object *sym_obj = HEAD(args);
  struct lisp_object *value = sym_obj->next;

  struct symbol *sym = symbol_lookup(SYM_NAME(sym_obj));

  if (!sym) {
    sym = get_new_symbol();
  }

  sym->symbol_name = SYM_NAME(sym_obj);
  sym->value = c_eval(value);

  return nil;
}

DEFUN("while", lisp_while, VAR_FIXED | UNEVAL_ARGS, 2) {
  struct lisp_object *cond = HEAD(args);
  struct lisp_object *body = cond->next;

  struct lisp_object *body_evaled = nil;

  while (TRUEP(c_eval(cond))) {
    if (body) {
      body_evaled = c_eval(body);
    }
  }

  return body_evaled;
}

DEFUN("progn", lisp_progn, VAR_MIN | UNEVAL_ARGS, 1) {
  struct lisp_object *current = HEAD(args);

  struct lisp_object *evaled = nil;

  while (current) {
    evaled = c_eval(current);

    current = current->next;
  }

  return evaled;
}

DEFUN("defun", lisp_defun, VAR_MIN | UNEVAL_ARGS, 3) {
  /* To define a function, first we need the parameters list and the forms */
  struct lisp_object *name = HEAD(args);
  struct lisp_object *params = name->next;
  struct lisp_object *forms = params->next;

  /* Separate the params and forms */
  params->next = NULL;
  forms->prev = NULL;

  /*
   * TODO: Set up the forms/params in a function object and insert it in a
   * symbol.
   */
  struct lisp_function *func = malloc(sizeof(struct lisp_function));
  func->params = params;
  func->forms = forms;

  func->numparams = list_length(params);

  struct symbol *sym = get_new_symbol();

  sym->symbol_name = SYM_NAME(name);
  sym->value = make_lisp_object(FUNCTION, func);

  return name;
}

void base_initialize() {
  lisp_if_init();
  lisp_lt_init();
  lisp_gt_init();
  lisp_eq_init();
  lisp_print_init();
  lisp_prints_init();
  lisp_setq_init();
  lisp_progn_init();
  lisp_while_init();
  lisp_defun_init();
}
