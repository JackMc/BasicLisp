/*
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "lisp.h"

#include "builtins_base.h"
#include "builtins_math.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

struct lisp_object *t;
struct lisp_object *nil;

struct symbol *symbol_table;
int symbol_table_counter;
static size_t symbol_table_size;

struct symbol *local_symbols;
size_t local_symbols_counter;

char *glob_error;

void register_builtins() {
  base_initialize();
  math_initialize();
}

void lisp_initialize() {
  /* We don't have a context, so the local symbol table is NULL */
  local_symbols = NULL;
  local_symbols_counter = 0;

  /* Initialize the symbol table and add nil and t. */
  symbol_table = malloc(sizeof(struct symbol)*SYMBOL_TABLE_INITIAL_SIZE);
  symbol_table_size = SYMBOL_TABLE_INITIAL_SIZE;
  symbol_table_counter = 0;

  /* Only object of type T_TYPE ever constructed. */
  t = make_lisp_object(T_TYPE, NULL);

  struct symbol *t_symbol = get_new_symbol();
  t_symbol->symbol_name = "t";
  t_symbol->value = t;
  t_symbol->builtin = C_TRUE;

  /* Nil - The FALSE value in Lisp. Equivalent in value to an empty list. */
  /* With a list, NULL data means an empty list. */
  nil = make_lisp_object(LIST, NULL);
  
  struct symbol *nil_symbol = get_new_symbol();
  nil_symbol->symbol_name = "nil";
  nil_symbol->value = nil;
  nil_symbol->builtin = C_TRUE;

  glob_error = NULL;

  /* Register builtin functions */
  register_builtins();
}

struct lisp_object *lisp_object_deep_copy(struct lisp_object *obj) {
  struct lisp_object *ret = make_lisp_object(obj->obj_type, NULL);

  switch (obj->obj_type) {
  case LIST:
  {
    if (!(obj->data)) {
      ret->data = NULL;
      return ret;
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

      new_prev = new_current;
      current = current->next;
    }

    ret->data = new_head;
    break;
  }
  case INTEGER:
  {
    LISPINT *data = malloc(sizeof(LISPINT));
    *data = TOLINT(obj);

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
    return t;
  }
  default:
    break;
  }

  return ret;
}

struct symbol *get_new_symbol() {
  if (symbol_table_counter == symbol_table_size) {
    /* Extend the symbol table by the scale factor */
    symbol_table_size *= SYMBOL_TABLE_SCALE_FACTOR;
    symbol_table = realloc(symbol_table, symbol_table_size);
  }

  struct symbol *sym = &(symbol_table[symbol_table_counter++]);

  sym->builtin = C_FALSE;

  return sym;
}

struct symbol *symbol_lookup(char *key) {
  int i;

  if (local_symbols) {
    for (i = 0; i < local_symbols_counter; i++) {
      if (strcmp(local_symbols[i].symbol_name, key) == 0) {
        /* Return a pointer. */
        return local_symbols + i;
      }
    }
  }

  for (i = 0; i < symbol_table_counter; i++) {
    if (strcmp(symbol_table[i].symbol_name, key) == 0) {
      /* Return a pointer. */
      return symbol_table + i;
    }
  }

  return NULL;
}

struct lisp_object *symbol_value(char *key) {
  struct symbol *sym = symbol_lookup(key);

  return sym ? sym->value : NULL;
}

void set_local_symbols(struct symbol *syms, size_t length) {
  local_symbols = syms;
  local_symbols_counter = length;
}

void unset_local_symbols() {
  local_symbols = NULL;
  local_symbols_counter = 0;
}

void define_builtin_function(char *symbol_name, enum paramspec spec, int min_params, int max_params,
			     struct lisp_object* (*func)(struct lisp_object*), C_BOOL is_builtin) {
  /* Handles the allocation of the symbol from the pool */
  struct symbol *sym = get_new_symbol();
  
  struct lisp_builtin *builtin = malloc(sizeof(struct lisp_builtin));
  builtin->func = func;
  builtin->spec = spec;
  builtin->min_params = min_params;
  builtin->max_params = max_params;

  sym->symbol_name = symbol_name;
  sym->value = make_lisp_object(BUILTIN, builtin);
  sym->builtin = is_builtin;
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
  default:
    return C_TRUE;
  }
}

struct lisp_object *make_lisp_object(enum type obj_type, void *data) {
  struct lisp_object *obj = malloc(sizeof(struct lisp_object));

  obj->obj_type = obj_type;
  obj->data = data;
  obj->next = NULL;
  obj->prev = NULL;
  obj->quoted = C_FALSE;
  obj->counter = 0;

  CLAIM(obj);

  return obj;
}

void set_error(char *error, ...) {
  va_list va;
  va_start(va, error);

  /* The maximum length for an error message is MAX_ERROR. It will be cut off after that. */
  char *buf = malloc(MAX_ERROR*sizeof(char));

  vsnprintf(buf, MAX_ERROR, error, va);

  glob_error = buf;
}

char *get_error() {
  return glob_error;
}

C_BOOL has_error() {
  return glob_error != NULL;
}

void lisp_error() {
  if (has_error()) {
    fprintf(stderr, "%s", get_error());
  }
  else {
    fprintf(stderr, "An unspecified error occurred.");
  }
  return;
}

C_BOOL symbol_reassign(struct symbol *sym, char *name, struct lisp_object *obj) {
  if (!obj) {
    // We are gonna rely on the fact that it was set in c_eval
    return C_FALSE;
  }
  else if (sym->builtin) {
    set_error("Symbol %s is a fundamental constant or builtin and cannot be modified.", sym->symbol_name);
    return C_FALSE;
  }

  sym->symbol_name = name;
  sym->value = obj;
  return C_TRUE;
}
