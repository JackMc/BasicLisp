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

#include <stdio.h>
#include <stdlib.h>

struct lisp_object *c_eval(struct lisp_object *obj) {
  if (!obj) {
    lisp_error();
  }
  
  if (obj->quoted) {
    struct lisp_object *new_obj = lisp_object_deep_copy(obj);
    new_obj->quoted = C_FALSE;
    return new_obj;
  }

  switch (obj->obj_type) {
  case LIST:
  {
    struct lisp_object *ret = make_lisp_object(LIST, NULL);
    
    struct lisp_object *head = HEAD(obj);

    if (!head) {
      // It already is nil
      return ret;
    }

    if (head->obj_type != SYMBOL || head->quoted) {
      set_error("First item of list to be evaluated must be a non-quoted symbol.");
      return NULL;
    }

    struct lisp_object *func = c_eval(head);

    if (!func) {
      set_error("Function %s doesn't exist.", TOSTR(head));
      return NULL;
    }

    if (func->obj_type != BUILTIN && func->obj_type != FUNCTION) {
      set_error("Symbol %s isn't a function.", SYM_NAME(head));
      return NULL;
    }

    /* Allocate an object to be used to store the copied arguments list */
    struct lisp_object *args = make_lisp_object(LIST, NULL);
    
    if (head->next) {
      struct lisp_object *args_head;

      if (func->obj_type == BUILTIN && (TOBUILTIN(func)->spec & UNEVAL_ARGS)) {
        args_head = lisp_object_deep_copy(head->next);
      }
      else {
	       args_head = c_eval(head->next);

         if (!args_head) {
           return NULL;
         }
      }

      args_head->next = NULL;
      args_head->prev = NULL;
      struct lisp_object *current = head->next->next;

      struct lisp_object *args_current = NULL;

      struct lisp_object *args_prev = args_head;

      while (current) {
        if (func->obj_type == BUILTIN && (TOBUILTIN(func)->spec & UNEVAL_ARGS)) {
          args_current = lisp_object_deep_copy(current);
        }
        else {
          args_current = c_eval(current);
          if (!args_current) {
            return NULL;
          }
        }

        args_current->prev = args_prev;
        args_current->next = NULL;
        args_prev->next = args_current;

        args_prev = args_current;
        current = current->next;
      }

      /* Finish constructing the arguments list */
      args->data = args_head;
    }

    /* Perform the function call. */
    if (func->obj_type == BUILTIN) {
      int count = list_length(args);

      struct lisp_builtin *builtin = TOBUILTIN(func);

      if ((builtin->spec & VAR_FIXED) && builtin->params != count) {
        set_error("Incorrect number of arguments (%d) to function %s!", count, TOSTR(head));
        return NULL;
      }
      else if ((builtin->spec & VAR_MIN) && count < builtin->params) {
        set_error("Incorrect number of arguments (%d) to function %s!", count, TOSTR(head));
        return NULL;
      }
      else if ((builtin->spec & VAR_MAX) && count > builtin->params) {
        set_error("Incorrect number of arguments (%d) to function %s!", count, TOSTR(head));
        return NULL;
      }

      return builtin->func(args);
    }
    else {
      struct lisp_function *func_obj = TOFUNC(func);

      int count = list_length(args);

      if (count != func_obj->numparams) {
        set_error("Incorrect number of arguments (%d) to function %s!", count, TOSTR(head));
        return NULL;
      }

      int i = 0;
      struct lisp_object *params_current = HEAD(func_obj->params);
      struct lisp_object *args_current = HEAD(args);
      struct symbol *syms = malloc(sizeof(struct symbol)*count);

      while (params_current) {
        syms[i].symbol_name = SYM_NAME(params_current);
        syms[i].value = args_current;

        i++;
        params_current = params_current->next;
        args_current = args_current->next;
      }

      set_local_symbols(syms, count);

      struct lisp_object *form_current = func_obj->forms;

      struct lisp_object *sub = nil;

      while (form_current) {
        sub = c_eval(form_current);

        if (!sub) {
          return NULL;
        }

        form_current = form_current->next;
      }

      unset_local_symbols();

      free(syms);

      return ret;
    }
  }
  case SYMBOL:
  {
    /* Do a lookup of the symbol and return the value. */
    struct lisp_object *value = symbol_value(SYM_NAME(obj));

    if (!value) {
      set_error("Symbol %s does not exist!", SYM_NAME(obj));
    }

    return value;
  }
  default:
  {
    return lisp_object_deep_copy(obj);
  }
  }
}
