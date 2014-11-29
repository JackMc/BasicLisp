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

#include <stdlib.h>
#include <ctype.h>

// Checks if there was an end of file while calling getc
#define READ_FGETC(inp, c) do {\
c = fgetc(inp);\
if (feof(inp)) { \
set_error("Unexpected end-of-file while parsing."); \
return NULL; \
} \
} while(0)

static struct lisp_object *mk_symbol(struct lisp_object *ret, FILE *input, char initial) {
  /* Make a base buffer with a reasonable estimate of how big the
   * symbol will be.
   */
  ungetc(initial, input);
  char *text = malloc(BASE_SYMBOL_LENGTH*sizeof(char));
  size_t text_size = BASE_SYMBOL_LENGTH;

  char in;
  int i = 0;
  READ_FGETC(input, in);

  while (!isspace(in) && in != ')') {
    if (i == text_size) {
      text_size *= SYMBOL_SCALE_FACTOR;
      text = realloc(text, text_size);
    }

    text[i] = in;
    i++;

    READ_FGETC(input, in);
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

  return ret;
}

struct lisp_object *c_read(FILE *input) {
  if (input == NULL) {
    input = stdin;
  }

  struct lisp_object *ret = malloc(sizeof(struct lisp_object));

  char initial;
  READ_FGETC(input, initial);

  /* Skip any whitespace */
  while (isspace(initial)) {
    READ_FGETC(input, initial);
  }

  /* Terminates the current list. */
  if (initial == ')') {
    return NULL;
  }

  if (initial == '\'') {
    ret->quoted = C_TRUE;
    /* Read the next character */
    READ_FGETC(input, initial);
  }

  /* List */
  if (initial == '(') {
    ret->obj_type = LIST;

    /* Recursive calls to do another sub-read until we get a NULL
     *  (')' character).
     */
    struct lisp_object *head = c_read(input);

    ret->data = head;
    struct lisp_object *current = NULL;
    struct lisp_object *prev = head;

    /* Some special handling for if the list is empty ('()') */
    if (head == NULL) {
      /* NULL data in a LIST type means () or nil. */
      ret->data = NULL;
      return ret;
    }

    while ((current = c_read(input))) {
      current->prev = prev;
      current->prev->next = current;
      current->next = NULL;
      prev = current;
    }
  }

  /* Integers */
  else if (initial == '-' || isdigit(initial)) {
    /* There is a special case in that "-" can be a symbol */
    if (initial == '-') {
      char checker;

      READ_FGETC(input, checker);


      /* Therefore, we check if the - is immediately followed by whitespace */
      if (isspace(checker)) {
        ungetc(checker, input);

        if (!mk_symbol(ret, input, initial)) {
          set_error("Unexpected end-of-file while parsing.");
          return NULL;
        }

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
    READ_FGETC(input, in);

    while (in != '"') {
      if (i == text_size) {
      	text_size *= STRINGBUF_SCALE_FACTOR;
      	text = realloc(text, text_size);
      }

      text[i] = in;
      i++;

      READ_FGETC(input, in);
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
    if (!mk_symbol(ret, input, initial)) {
      set_error("Unexpected end-of-file while parsing.");
      return NULL;
    }
  }

  return ret;
}
