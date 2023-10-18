#include <ruby.h>

VALUE new_path(VALUE, VALUE);
VALUE to_path(VALUE *list, int depth);
VALUE append_long_to_path(VALUE, long);
