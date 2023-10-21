#include <ruby.h>
#include "keywords.h"
#include "compiled_schema.h"
#include "types/context.h"
#include "error.h"

extern bool is_valid(VALUE, CompiledSchema *, VALUE, Context *);

static CompiledSchema *root_schema(VALUE self) {
  CompiledSchema *compiled_schema;
  VALUE compiled_schema_obj = rb_ivar_get(self, rb_intern("compiled_schema"));

  return GetCompiledSchema(compiled_schema_obj, compiled_schema);
}

static Context initial_context() {
  Context context;

  context.path[0] = root_path_str;
  context.depth = 0;
  context.env = (ValidationEnv){ false };

  return context;
}

VALUE rb_validate_with_schema(VALUE self, VALUE compiled_schema_obj, VALUE data) {
  if (!rb_block_given_p()) {
    VALUE args[2] = { compiled_schema_obj, data };

    RETURN_SIZED_ENUMERATOR(self, 2, args, 0);
  }

  CompiledSchema *compiled_schema;
  GetCompiledSchema(compiled_schema_obj, compiled_schema);

  Context context_s;

  context_s.path[0] = root_path_str;
  context_s.depth = 0;
  context_s.env =  (ValidationEnv){ false };

  compiled_schema->validation_function(self, compiled_schema, data, &context_s);

  return Qnil;
}

VALUE rb_validate(VALUE self, VALUE data) {
  VALUE compiled_schema_obj = rb_ivar_get(self, rb_intern("compiled_schema"));

  return rb_funcall(self, rb_intern("validate_with_schema"), 2, compiled_schema_obj, data);
}

VALUE rb_valid(VALUE self, VALUE data) {
  CompiledSchema *schema = root_schema(self);
  Context context = initial_context();

  bool valid = is_valid(self, schema, data, &context);

  return valid ? Qtrue : Qfalse;
}

VALUE rb_compile(VALUE self) {
  VALUE is_compiled = rb_ivar_get(self, rb_intern("compiled"));

  if(is_compiled != Qtrue) compile_schema(self);

  return self;
}

void Init_schema() {
  VALUE fast_json_class = rb_const_get(rb_cObject, rb_intern("FastJSON"));
  VALUE schema_class = rb_const_get(fast_json_class, rb_intern("Schema"));

  Init_keywords();
  Init_error(schema_class);
  Init_compiled_schema(schema_class);

  rb_define_method(schema_class, "compile", rb_compile, 0);
  rb_define_method(schema_class, "validate", rb_validate, 1);
  rb_define_method(schema_class, "valid?", rb_valid, 1);
  rb_define_private_method(schema_class, "validate_with_schema", rb_validate_with_schema, 2);
}
