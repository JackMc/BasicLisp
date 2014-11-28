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

static void c_print_internal(struct lisp_object *obj) {
  if (!obj) {
    lisp_error();
    return;
  }
  
  switch (obj->obj_type) {
  case LIST:
  {
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
  }
  case INTEGER:
    printf(LISPINT_FORMAT, TOLINT(obj));
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
  case FUNCTION:
    printf("<function at %p>", obj);
    break;
  case BUILTIN:
    printf("<builtin at %p>", obj);
    break;
  default:
    printf("<object at %p>", obj);
    break;
  }
}

struct lisp_object *c_print(struct lisp_object *obj) {
  c_print_internal(obj);

  printf("\n");

  return nil;
}
