#include "lisp.h"

#include <stdlib.h>
#include <string.h>

struct lisp_object *t;
struct lisp_object *nil;

struct symbol *symbol_table;
static int symbol_table_counter;
static int symbol_table_size;

void register_builtins() {
  base_initialize();
  /* arith_initialize(); */
}

static struct symbol *get_new_symbol();

void lisp_initialize() {
  /* Initialize the symbol table and add nil and t. */

  symbol_table = malloc(sizeof(struct symbol)*SYMBOL_TABLE_INITIAL_SIZE);
  symbol_table_size = SYMBOL_TABLE_INITIAL_SIZE;
  symbol_table_counter = 0;

  /* Only object of type T_TYPE ever constructed. */
  t = malloc(sizeof(struct lisp_object));
  t->obj_type = T_TYPE;
  t->prev = NULL;
  t->next = NULL;
  t->quoted = C_FALSE;
  /* Data is never accessed. */
  t->data = NULL;
  struct symbol *t_symbol = get_new_symbol();
  t_symbol->symbol_name = "t";
  t_symbol->value = t;

  /* Nil - The FALSE value in Lisp. Equivalent in value to an empty list. */
  nil = malloc(sizeof(struct lisp_object));
  nil->obj_type = LIST;
  nil->prev = NULL;
  nil->next = NULL;
  nil->quoted = C_FALSE;
  /* With a list, NULL data means an empty list. */
  nil->data = NULL;
  struct symbol *nil_symbol = get_new_symbol();
  nil_symbol->symbol_name = "nil";
  nil_symbol->value = nil;

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

    while (current != NULL) {
      new_current = lisp_object_deep_copy(current);
      new_prev->next = new_current;
      new_current->prev = new_prev;

      current = current->next;
    }

    ret->data = new_head;
    break;
  }
  case INTEGER:
  {
    LISPINT *data = malloc(sizeof(LISPINT));
    *data = *((int*)(obj->data));
    
    ret->data = data;
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

    ret->data = new_data;
    
    break;
  }
  case T_TYPE:
  {
    ret->data = NULL;
    return obj;
    break;
  }
  }
  return ret;
}

static struct symbol *get_new_symbol() {
  if (symbol_table_counter == symbol_table_size) {
    /* Extend the symbol table by the scale factor */
    symbol_table_size *= SYMBOL_TABLE_SCALE_FACTOR;
    symbol_table = realloc(symbol_table, symbol_table_size);
  }
  
  return &(symbol_table[symbol_table_counter++]);
}

struct lisp_object *symbol_lookup(char *key) {
  int i;

  for (i = 0; i < symbol_table_counter; i++) {
    if (strcmp(symbol_table[i].symbol_name, key) == 0) {
      return symbol_table[i].value;
    }
  }

  return NULL;
}

void define_builtin_function(char *symbol_name, enum paramspec spec, int numparams,
			     struct lisp_object* (*func)(struct lisp_object*)) {
  /* Handles the allocation of the symbol from the pool */
  struct symbol *sym = get_new_symbol();

  struct lisp_object *obj = malloc(sizeof(struct lisp_object));
  struct lisp_builtin *builtin = malloc(sizeof(struct lisp_builtin));
  builtin->func = func;
  builtin->spec = spec;
  builtin->params = numparams;

  obj->obj_type = BUILTIN;
  obj->data = builtin;
  obj->next = NULL;
  obj->prev = NULL;

  sym->symbol_name = symbol_name;
  sym->value = obj;
}

int list_length(struct lisp_object *list) {
  struct lisp_object *current = HEAD(list);

  int count = 0;

  while (current) {
    count++;
    
    current = current->next;
  }

  return count;
}

/*
 * Checks if object `obj` is 'true' by the conditions
 */
C_BOOL true_p(struct lisp_object *obj) {
  switch (obj->obj_type) {
  case INTEGER:
    if (!TOLINT(obj)) {
      return C_FALSE;
    }
  case STRING:
    if (!strlen(TOSTR(obj))) {
      return C_FALSE;
    }
  case LIST:
    if (!(obj->data)) {
      return C_FALSE;
    }
  }

  return C_TRUE;
}
