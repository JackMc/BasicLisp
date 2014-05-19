#include "lisp.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

static void mk_symbol(struct lisp_object *ret, FILE *input, char initial) {
  /* Make a base buffer with a reasonable estimate of how big the
   * symbol will be.
   */
  ungetc(initial, input);
  char *text = malloc(BASE_SYMBOL_LENGTH*sizeof(char));
  size_t text_size = BASE_SYMBOL_LENGTH;

  char in;
  int i = 0;
  in = fgetc(input);

  while (!isspace(in) && in != ')') {
    if (i == text_size) {
      text_size *= SYMBOL_SCALE_FACTOR;
      text = realloc(text, text_size);
    }

    text[i] = in;
    i++;

    in = fgetc(input);
  }

  ungetc(in, input);

  if (i == text_size) {
    text_size += 1;
    text = realloc(text, text_size);
  }
  text[i] = '\0';

  /* Symbol name is now in text */
  ret->obj_type = SYMBOL;
  ret->data = text;
}

struct lisp_object *c_read(FILE *input) {
  if (input == NULL) {
    input = stdin;
  }

  struct lisp_object *ret = malloc(sizeof(struct lisp_object));

  char initial = fgetc(input);

  /* Skip any whitespace */
  while (isspace(initial)) {
    initial = fgetc(input);
  }

  if (feof(input)) {
    fprintf(stderr, "Unexpected end-of-file while parsing.");

    /* TODO: Do something a little less radical here... */
    exit(1);

    return NULL;
  }

  /* Terminates the current list. */
  if (initial == ')') {
    return NULL;
  }

  if (initial == '\'') {
    ret->quoted = C_TRUE;
    /* Read the next character */
    initial = fgetc(input);
  }

  /* List */
  if (initial == '(') {
    ret->obj_type = LIST;

    /* Recursive calls to do another sub-read until we get a NULL
     *  (')' character).
     */
    struct lisp_object *head = c_read(input);
    /* Some special handling for if the list is empty ('()') */
    if (head == NULL) {
      /* NULL data in a LIST type means () or nil. */
      ret->data = NULL;
      return ret;
    }

    ret->data = head;
    struct lisp_object *current = NULL;
    struct lisp_object *prev = head;

    while (current = c_read(input)) {
      current->prev = prev;
      current->prev->next = current;
      current->next = NULL;
      prev = current;
    }
  }

  /* Integers */
  else if (initial == '-' || isdigit(initial)) {
    if (initial == '-') {
      char checker = fgetc(input);

      if (isspace(checker)) {
        ungetc(checker, input);

        mk_symbol(ret, input, initial);

        return ret;
      }

      ungetc(checker, input);
    }

    /* Return the character to the stream and read it as an int. */
    ungetc(initial, input);
    LISPINT *data = malloc(sizeof(LISPINT));
    fscanf(input, LISPINT_FORMAT, data);
    ret->obj_type = INTEGER;
    ret->data = data;
  }
  /* String */
  else if (initial == '"') {
    char *text = malloc(BASE_STRINGBUF_LENGTH*sizeof(char));
    size_t text_size = BASE_STRINGBUF_LENGTH;

    char in;
    int i = 0;
    in = fgetc(input);

    while (in != '"') {
      if (i == text_size) {
      	text_size *= STRINGBUF_SCALE_FACTOR;
      	text = realloc(text, text_size);
      }

      text[i] = in;
      i++;

      in = fgetc(input);
    }

    /* Terminate the string */
    if (i == text_size) {
      text_size += 1;
      text = realloc(text, text_size);
    }
    text[i] = '\0';

    ret->obj_type = STRING;
    ret->data = text;
  }
  /* Symbol */
  else {
    mk_symbol(ret, input, initial);
  }

  return ret;
}
