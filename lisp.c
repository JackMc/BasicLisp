#include "lisp.h"

#include <stdlib.h>
#include <string.h>

struct lisp_object *t;
struct lisp_object *nil;

struct symbol *symbol_table;
static int symbol_table_counter = 0;

void register_builtins() {
  base_initialize();
  arith_initialize();
}

void lisp_initialize() {
  /* Only object of type T_TYPE ever constructed. */
  t = malloc(sizeof(struct lisp_object));
  t->obj_type = T_TYPE;
  t->prev = NULL;
  t->next = NULL;
  t->quoted = C_FALSE;
  /* Data is never accessed. */
  t->data = NULL;

  /* Nil - The FALSE value in Lisp. Equivalent in value to an empty list. */
  nil = malloc(sizeof(struct lisp_object));
  nil->obj_type = LIST;
  nil->prev = NULL;
  nil->next = NULL;
  nil->quoted = C_FALSE;
  /* With a list, NULL data means an empty list. */
  nil->data = NULL;

  /* Initialize the symbol table and add nil and t. */

  symbol_table = malloc(sizeof(struct symbol)*2);
  symbol_table[0].symbol_name = "t";
  symbol_table[0].object = t;

  symbol_table = malloc(sizeof(struct symbol)*2);
  symbol_table[1].symbol_name = "nil";
  symbol_table[1].object = nil;

  /* Set up the prev/next pointers */
  symbol_table[0].prev = NULL;
  symbol_table[0].next = &symbol_table[1];
  symbol_table[1].prev = &symbol_table[0];
  symbol_table[1].next = NULL;

  /* Register builtin functions */
  register_builtins();
}

struct lisp_object *lisp_object_deep_copy(struct lisp_object *obj) {
  struct lisp_object *ret = malloc(sizeof(struct lisp_object));

  ret->obj_type = obj->obj_type;
  ret->quoted = obj->quoted;
  ret->prev = NULL;
  ret->next = NULL;

  switch (obj->obj_type) {
  case LIST:
  {
    if (!(obj->data)) {
      ret->data = NULL;
    }

    struct lisp_object *current = (struct lisp_object*)(obj->data);
    struct lisp_object *new_head = lisp_object_deep_copy(current);
    /* Move the current pointer forward so the below loop only works on elements that need copies */
    current = current->next;
    struct lisp_object *new_current = NULL;
    struct lisp_object *new_prev = new_head;

    while (current) {
      new_current = lisp_object_deep_copy(current);
      new_prev->next = new_current;
      new_current->prev = new_prev;
    }
    
    current = current->next;
    break;
  }
  case INTEGER:
  {
    LISPINT *data = malloc(sizeof(LISPINT));
    *data = *((int*)(obj->data));
    break;
  }
  /* String and symbol are the same data representation. */
  case STRING:
  case SYMBOL:
  {
    char *old_data = (char*)(obj->data);
    /* Length of string plus one for a '\0' */
    char *new_data = malloc((strlen(old_data)+1)*sizeof(char));
    /* This copy is safe because we just made the buffers exactly equal in size ^^ */
    strcpy(new_data, old_data);
    break;
  }
  case T_TYPE:
  {
    return obj;
    break;
  }
  }
}

void define_builtin_function(char *symbol_name, enum paramspec spec, int numparams,
			     struct lisp_object* (*func)(struct lisp_object*)) {
  struct lisp_object *obj = malloc(sizeof(struct lisp_object*));

  obj->obj_type = BUILTIN;
  obj->data = func;
  obj->next = NULL;
  obj->prev = NULL;
}
