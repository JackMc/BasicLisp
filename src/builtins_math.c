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
#include <math.h>

DEFUN("+", lisp_add, EVAL_ARGS, 2, -1) {
  LISPINT *sum = malloc(sizeof(LISPINT));

  struct lisp_object *current = HEAD(args);

  while (current) {
    if (current->obj_type != INTEGER) {
      set_error("Incorrect argument types to '+'");
      return NULL;
    }

    (*sum) += TOLINT(current);
    current = current->next;
  }

  return make_lisp_object(INTEGER, sum);
}

DEFUN("-", lisp_subtract, EVAL_ARGS, 2, -1) {
  LISPINT *sum = malloc(sizeof(LISPINT));

  struct lisp_object *current = HEAD(args);

  (*sum) = TOLINT(current);

  current = current->next;

  while (current) {
    if (current->obj_type != INTEGER) {
      set_error("Incorrect argument types to '-'");
      return NULL;
    }

    (*sum) -= TOLINT(current);
    current = current->next;
  }

  return make_lisp_object(INTEGER, sum);
}

DEFUN("*", lisp_multiply, EVAL_ARGS, 2, -1) {
  LISPINT *ret = malloc(sizeof(LISPINT));

  struct lisp_object *current = HEAD(args);

  (*ret) = TOLINT(current);

  current = current->next;

  while (current) {
    if (current->obj_type != INTEGER) {
      set_error("Incorrect argument types to '*'");
      return NULL;
    }

    (*ret) *= TOLINT(current);
    current = current->next;
  }

  return make_lisp_object(INTEGER, ret);
}

DEFUN("/", lisp_divide, EVAL_ARGS, 2, -1) {
  LISPINT *ret = malloc(sizeof(LISPINT));

  struct lisp_object *current = HEAD(args);

  (*ret) = TOLINT(current);

  current = current->next;

  while (current) {
    if (current->obj_type != INTEGER) {
      set_error("Incorrect argument types to '/'");
      return NULL;
    }

    (*ret) /= TOLINT(current);
    current = current->next;
  }

  return make_lisp_object(INTEGER, ret);
}

DEFUN("%", lisp_mod, EVAL_ARGS, 2, 2) {
  LISPINT *ret = malloc(sizeof(LISPINT));

  struct lisp_object *first = HEAD(args);

  struct lisp_object *second = first->next;

  if (first->obj_type != INTEGER || second->obj_type != INTEGER) {
    set_error("Incorrect argument types to '%'");
    return NULL;
  }

  (*ret) = TOLINT(first) % TOLINT(second);

  return make_lisp_object(INTEGER, ret);
}

DEFUN("^", lisp_power, EVAL_ARGS, 2, 2) {
  LISPINT * ret = malloc(sizeof(LISPINT));

  struct lisp_object *first = HEAD(args);

  struct lisp_object *second = first->next;

  if (first->obj_type != INTEGER || second->obj_type != INTEGER) {
    set_error("Incorrect argument types to '^'");
    return NULL;
  }

  (*ret) = pow(TOLINT(first), TOLINT(second));

  return make_lisp_object(INTEGER, ret);
}

void math_initialize() {
  lisp_add_init();
  lisp_subtract_init();
  lisp_multiply_init();
  lisp_divide_init();
  lisp_mod_init();
  lisp_power_init();
}
