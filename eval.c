#include "lisp.h"

#include <stdio.h>
#include <stdlib.h>

struct lisp_object *c_eval(struct lisp_object *obj) {
  if (obj->quoted) {
    struct lisp_object *new_obj = lisp_object_deep_copy(obj);
    new_obj->quoted = C_FALSE;
    return new_obj;
  }

  struct lisp_object *ret = malloc(sizeof(struct lisp_object*));
  ret->prev = NULL;
  ret->next = NULL;

  switch (obj->obj_type) {
  case LIST:
  {
    struct lisp_object *head = HEAD(obj);
    if (head->obj_type != SYMBOL || head->quoted) {
      fprintf(stderr, "First item of list to be function called must be a non-quoted symbol.");
      exit(1);
    }

    struct lisp_object *func = c_eval(head);

    struct lisp_object *args = malloc(sizeof(struct lisp_object));
    args->prev = NULL;
    args->next = NULL;
    args->data = NULL;
    args->obj_type = LIST;

    if (head->next) {
      struct lisp_object *args_head;
      if (!func) {
	fprintf(stderr, "Function %s doesn't exist.\n", TOSTR(head));
	exit(1);
      }
      if (func->obj_type == BUILTIN && (TOBUILTIN(func)->spec & UNEVAL_ARGS)) {
	args_head = lisp_object_deep_copy(head->next);
      }
      else {
	args_head = c_eval(head->next);
      }
      
      args_head->next = NULL;
      args_head->prev = NULL;
      struct lisp_object *current = head->next->next;

      struct lisp_object *args_current = NULL;

      struct lisp_object *args_prev = args_head;

      while (current) {
	if (func->obj_type == BUILTIN && (TOBUILTIN(func)->spec & UNEVAL_ARGS)) {
	  args_current = lisp_object_deep_copy(current);
	}
	else {
	  args_current = c_eval(current);
	}

	args_current->prev = args_prev;
	args_current->next = NULL;
	args_prev->next = args_current;

	args_prev = args_current;
	current = current->next;
      }

      /* Finish constructing the arguments list */
      args->data = args_head;

      if (func->obj_type == BUILTIN) {
	struct lisp_builtin *builtin = TOBUILTIN(func);

	return builtin->func(args);
      }
      else {
	/* TODO: Perform a normal function call */
      }
    }
  }
  case SYMBOL:
  {
    /* Do a lookup of the symbol and return the value. */
    return symbol_lookup((char*)(obj->data));
  }
  default:
  {
    return lisp_object_deep_copy(obj);
  }
  }
}
