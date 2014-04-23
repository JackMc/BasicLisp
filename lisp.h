#ifndef LISP_H_
#define LISP_H_

#include <stdio.h>

#define C_BOOL int
#define C_TRUE 1
#define C_FALSE 0

enum type {
  INTEGER, /* LISPINT * */
  STRING, /* char * */
  SYMBOL, /* char * */
  LIST, /* struct lisp_object * or NULL for empty list. */
  FUNC,	/* struct lisp_object * */
  BUILTIN, /* struct lisp_object* (*func)(struct lisp_object*) */
  T_TYPE /* NULL */
};

struct lisp_object {
  void *data;

  struct lisp_object *prev;
  struct lisp_object *next;

  enum type obj_type;

  C_BOOL quoted;
};

extern struct lisp_object *t;
extern struct lisp_object *nil;

struct symbol {
  struct symbol *prev;
  struct symbol *next;

  char *symbol_name;
  struct lisp_object *object;
};

extern struct symbol *symbol_table;

/*
 * Initialize the Lisp interpreter symbol table and builtins.
 */
void lisp_initialize();

/* 
 * The C implementation of the Lisp `read` function.
 * Input: FILE *input - Where to read the text for the reader from. NULL means stdin.
 * Output: lisp_object* - The root of the unevaluated objects returned.
 */
struct lisp_object *c_read(FILE *input);

/*
 * The C implementation of the Lisp `eval` function.
 * Input: lisp_object *root - The root of the Lisp object to be evaluated. Usually output from read()
 * Output: lisp_object* - The result of the Lisp object's evaluation.
 */
struct lisp_object *c_eval(struct lisp_object *root);

/*
 * The C implementation of the Lisp `print` function.
 * Input: lisp_object *obj - The object to print.
 * Output: nil.
 */
struct lisp_object *c_print(struct lisp_object *obj);

/* 
 * Does a deep copy of a lisp_object structure. This should be avoided if possible.
 * Input: lisp_object *obj - An object to be deep copied
 * Output: lisp_object* - A deep copy of obj.
 */
struct lisp_object *lisp_object_deep_copy(struct lisp_object *obj);

/* 
 * Defines the way parameters are restricted.
 */
enum paramspec {
  FIXED, /* The number of parameters must be equal to numparams */
  VAR_MIN, /* Defines the function as having a minimum number of arguments, as defined by numparams */
  VAR_MAX /* Defines the function as having a maximum number of arguments, as defined by numparams */
};

/* 
 * Defines a builtin function. Only called from C.
 * Input:
 * char *symbol_name - The name to be used to call this function in Lisp.
 * enum paramspec spec - Defines the way parameters are restricted.
 * int numparams - Number of parameters - Exact interpretation depends on spec. See above paramspec docs.
 * lisp_object* (*func)(lisp_object*) - The C function you are defining. Takes a lisp_object which is always a 
 *                                      LIST of the passed-in parameters. Returns a lisp_object which will be 
 *                                      returned back to the Lisp code.
 */
void define_builtin_function(char *symbol_name, enum paramspec spec, int numparams,
			     struct lisp_object* (*func)(struct lisp_object*));

/* Macro to define a function from C. This requires that this be called in register_builtins() in lisp.c */
#define DEFUN(lisp_name, func_name, spec, numparams) \
  lisp_object * func_name (lisp_object*);
  void func_name ## _init () { \
    define_builtin_function(lisp_name , spec , numparams , func_name ); \
  }

/* Possibly changing constants */
/* Not necessarily a max length for a symbol. It is much more efficient if a 
 * symbol's length is smaller than this.
 */
#define BASE_SYMBOL_LENGTH 35
/* A scaling factor for when a symbol's length is > BASE_SYMBOL_LENGTH */
#define SYMBOL_SCALE_FACTOR 2

/* Same definitions for string constants */
#define BASE_STRINGBUF_LENGTH 256
#define STRINGBUF_SCALE_FACTOR 2

#define LISPINT int
#define LISPINT_FORMAT "%d"

#endif /* LISP_H_ */
