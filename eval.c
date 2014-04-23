#include "lisp.h"

struct lisp_object *c_eval(struct lisp_object *obj) {
  if (obj->quoted) {
    return lisp_object_deep_copy(obj);
  }

  /* switch (obj->obj_type) { */
  /* case LIST: */

  /* Needs a function mechanism... */
  return lisp_object_deep_copy(obj);
}
