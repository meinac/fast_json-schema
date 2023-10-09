#include "error.h"
#include "path.h"

static VALUE error_class;

void yield_error(CompiledSchema *compiled_schema, VALUE data, Context *context, const char *type) {
  VALUE args[4];
  args[0] = compiled_schema->path;
  args[1] = data;
  args[2] = to_path(context->path, context->depth);
  args[3] = rb_str_new_cstr(type);

  VALUE error = rb_class_new_instance(4, args, error_class);

  rb_yield(error);
}

void Init_error(VALUE schema_class) {
  error_class = rb_const_get(schema_class, rb_intern("Error"));
}
