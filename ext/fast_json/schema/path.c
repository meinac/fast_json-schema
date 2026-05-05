#include "path.h"

VALUE new_path(VALUE root, VALUE name) {
  StringValue(root);
  StringValue(name);

  VALUE result = rb_str_dup(root);

  rb_str_cat_cstr(result, "/");
  rb_str_buf_append(result, name);

  return result;
}

VALUE to_path(VALUE *list, int depth) {
  int i;
  VALUE path = rb_str_new2("");

  for(i = 0; i <= depth; i++) {
    if(RB_TYPE_P(list[i], T_STRING)) {
      rb_str_append(path, list[i]);
    } else if (RB_INTEGER_TYPE_P(list[i])) {
      VALUE str = rb_sprintf("%ld", NUM2LONG(list[i]));

      rb_str_append(path, str);
    } else {
      rb_raise(rb_eRuntimeError, "Unexpected value while generating the path!");
    }

    rb_str_cat_cstr(path, "/");
  }

  return path;
}

VALUE append_long_to_path(VALUE root, long i) {
  StringValue(root);

  VALUE result = rb_str_dup(root);
  VALUE suffix = rb_sprintf("/%ld", i);
  rb_str_buf_append(result, suffix);

  return result;
}
