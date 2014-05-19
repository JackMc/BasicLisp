#include "lisp.h"

#include <stdio.h>

static void c_print_internal(struct lisp_object *obj) {
  switch (obj->obj_type) {
  case LIST:
    if (obj->quoted) {
      printf("'");
    }
    printf("(");
    struct lisp_object *current = (struct lisp_object *)(obj->data);
    while (current) {
      c_print_internal(current);

      if (current->next) {
	printf(" ");
      }

      current = current->next;
    }
    printf(")");
    break;
  case INTEGER:
    printf("%d", *((int*)(obj->data)));
    break;
  case STRING:
    printf("\"%s\"", (char*)(obj->data));
    break;
  case SYMBOL:
    if (obj->quoted) {
      printf("'");
    }
    printf("%s", (char*)(obj->data));
    break;
  case T_TYPE:
    printf("t");
    break;
  }
}

struct lisp_object *c_print(struct lisp_object *obj) {
  c_print_internal(obj);

  printf("\n");

  return nil;
}
