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

DEFUN("+", lisp_add, VAR_MIN, 2) {
  LISPINT *sum = malloc(sizeof(LISPINT));

  struct lisp_object *current = HEAD(args);

  while (current) {
    (*sum) += TOLINT(current);
    current = current->next;
  }

  return make_lisp_object(INTEGER, sum);
}

DEFUN("-", lisp_subtract, VAR_MIN, 2) {
  LISPINT *sum = malloc(sizeof(LISPINT));

  struct lisp_object *current = HEAD(args);

  (*sum) = TOLINT(current);

  current = current->next;

  while (current) {
    (*sum) -= TOLINT(current);
    current = current->next;
  }

  return make_lisp_object(INTEGER, sum);
}

DEFUN("*", lisp_multiply, VAR_MIN, 2) {
  LISPINT *ret = malloc(sizeof(LISPINT));

  struct lisp_object *current = HEAD(args);

  (*ret) = TOLINT(current);

  current = current->next;

  while (current) {
    (*ret) *= TOLINT(current);
    current = current->next;
  }

  return make_lisp_object(INTEGER, ret);
}

DEFUN("/", lisp_divide, VAR_MIN, 2) {
  LISPINT *ret = malloc(sizeof(LISPINT));

  struct lisp_object *current = HEAD(args);

  (*ret) = TOLINT(current);

  current = current->next;

  while (current) {
    (*ret) /= TOLINT(current);
    current = current->next;
  }

  return make_lisp_object(INTEGER, ret);
}

DEFUN("%", lisp_mod, VAR_FIXED, 2) {
  LISPINT *ret = malloc(sizeof(LISPINT));

  struct lisp_object *first = HEAD(args);

  struct lisp_object *second = first->next;

  (*ret) = TOLINT(first) % TOLINT(second);

  return make_lisp_object(INTEGER, ret);
}

void math_initialize() {
  lisp_add_init();
  lisp_subtract_init();
  lisp_multiply_init();
  lisp_divide_init();
  lisp_mod_init();
}
