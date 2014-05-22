#include "lisp.h"
#include "builtins_base.h"

#include <stdio.h>
#include <stdlib.h>

DEFUN("<", lisp_lt, VAR_FIXED, 2) {
  struct lisp_object *first = HEAD(args);
  struct lisp_object *second = first->next;

  if (first->obj_type != INTEGER || second->obj_type != INTEGER) {
    set_error("Can only compare integers with <.");
    return NULL;
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
    set_error("Can only compare integers with >.");
    return NULL;
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
    set_error("= can only compare integers.");
    return NULL;
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
    set_error("Incorrect number of arguments (%d) to function if!", length);
    return NULL;
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
  if (HEAD(args)->obj_type != STRING) {
    set_error("Argument to prints must be a string.");
    return NULL;
  }

  char *to_print = TOSTR(HEAD(args));

  printf("%s\n", to_print);

  return nil;
}

DEFUN("print", lisp_print, VAR_MIN, 1) {
  struct lisp_object *current = HEAD(args);

  while (current) {
    c_print(current);

    current = current->next;
  }

  return nil;
}

DEFUN("setq", lisp_setq, VAR_FIXED | UNEVAL_ARGS, 2) {
  struct lisp_object *sym_obj = HEAD(args);

  if (sym_obj->obj_type != SYMBOL) {
    set_error("First argument to setq must be a symbol reference.");
    return NULL;
  }

  /*
   * We evaluate first so that the expression cannot try to access the symbol
   * if it doesn't exist.
   */
  struct lisp_object *value = c_eval(sym_obj->next);

  struct symbol *sym = symbol_lookup(SYM_NAME(sym_obj));

  if (!sym) {
    sym = get_new_symbol();
  }
  else if (sym->constant) {
    set_error("Symbol %s is a fundamental constant and cannot be modified.", sym->symbol_name);
    return NULL;
  }

  sym->symbol_name = SYM_NAME(sym_obj);
  sym->value = value;

  return nil;
}

DEFUN("while", lisp_while, VAR_FIXED | UNEVAL_ARGS, 2) {
  struct lisp_object *cond = HEAD(args);
  struct lisp_object *body = cond->next;

  struct lisp_object *body_evaled = nil;

  struct lisp_object *current = c_eval(cond);

  if (!current) {
    return NULL;
  }

  while (TRUEP(current)) {
    if (body) {
      body_evaled = c_eval(body);
      if (!body_evaled) {
        return NULL;
      }
    }

    current = c_eval(cond);
    if (!current) {
      return NULL;
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

  struct lisp_function *func = malloc(sizeof(struct lisp_function));
  func->params = params;
  func->forms = forms;

  func->numparams = list_length(params);

  struct symbol *sym = symbol_lookup(SYM_NAME(name));

  if (!sym) {
    sym = get_new_symbol();
  }

  sym->symbol_name = SYM_NAME(name);
  sym->value = make_lisp_object(FUNCTION, func);

  return name;
}

DEFUN("symbols", lisp_symbols, VAR_FIXED, 0) {
  int i;

  struct lisp_object *head = make_lisp_object(LIST, NULL);

  struct lisp_object *prev = NULL;
  struct lisp_object *current = NULL;

  for (i = 0; i < symbol_table_counter; i++) {
      struct symbol *sym = symbol_table + i;

      struct lisp_object *symref = make_lisp_object(SYMBOL, sym->symbol_name);
      struct lisp_object *value = lisp_object_deep_copy(sym->value);
      symref->next = value;
      value->prev = symref;
      if (i == 0) {
        current = head;
        current->data = symref;
      }
      else {
        current = make_lisp_object(LIST, symref);
      }

      current->prev = prev;
      if (prev) {
        prev->next = current;
      }
      prev = current;
  }

  return make_lisp_object(LIST, head);
}

/* Returns the CAR (the first element) of the given list. */
DEFUN("car", lisp_car, VAR_FIXED, 1) {
  struct lisp_object *list = HEAD(args);

  if (list->obj_type != LIST) {
    set_error("Cannot take car of non-list.");
    return NULL;
  }
  if (list->data == NULL) {
    set_error("Cannot take car of nil.");
    return NULL;
  }

  struct lisp_object *ret = lisp_object_deep_copy(HEAD(list));

  ret->next = NULL;
  ret->prev = NULL;

  return ret;
}

DEFUN("cdr", lisp_cdr, VAR_FIXED, 1) {
  struct lisp_object *list = HEAD(args);

  if (list->obj_type != LIST) {
    set_error("Cannot take cdr of non-list.");
    return NULL;
  }
  if (list->data == NULL) {
    set_error("Cannot take cdr of nil.");
    return NULL;
  }

  struct lisp_object *old_head = HEAD(list);

  /* Temporarily screw up the old list to make deep copy work */
  list->data = HEAD(list)->next;

  /* Makes a copy of the list without the first element */
  struct lisp_object *cdr = lisp_object_deep_copy(list);

  list->data = old_head;

  return cdr;
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
  lisp_symbols_init();
  lisp_cdr_init();
  lisp_car_init();
}
