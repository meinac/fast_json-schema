#include "path.h"

VALUE new_path(VALUE root, VALUE name) {
  char *root_ptr = StringValueCStr(root);
  char *name_ptr = StringValueCStr(name);

  return rb_sprintf("%s/%s", root_ptr, name_ptr);
}

VALUE to_path(VALUE *list, int depth) {
  int i;
  VALUE path = rb_str_new2("");

  for(i = 0; i <= depth; i++) {
    if(RB_TYPE_P(list[i], T_STRING)) {
      rb_str_append(path, list[i]);
    } else if (RB_INTEGER_TYPE_P(list[i])) {
      VALUE str = rb_sprintf("%d", FIX2INT(list[i]));

      rb_str_append(path, str);
    } else {
      rb_raise(rb_eRuntimeError, "Unexpected value while generating the path!");
    }

    rb_str_cat_cstr(path, "/");
  }

  return path;
}
