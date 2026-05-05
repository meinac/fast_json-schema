#include <ruby.h>
#include "keywords.h"
#include "compiled_schema.h"
#include "types/context.h"
#include "error.h"
#include "is_valid.h"

extern bool is_valid(VALUE, CompiledSchema *, VALUE, Context *);

static CompiledSchema *root_schema(VALUE self) {
  CompiledSchema *compiled_schema;
  VALUE compiled_schema_obj = rb_ivar_get(self, rb_intern("compiled_schema"));

  return GetCompiledSchema(compiled_schema_obj, compiled_schema);
}

VALUE rb_validate(VALUE self, VALUE data) {
  if (!rb_block_given_p()) {
    VALUE args[1] = { data };

    RETURN_SIZED_ENUMERATOR(self, 1, args, 0);
  }

  CompiledSchema *schema = root_schema(self);
  Context *context;
  VALUE context_obj = create_context(&context);

  schema->validation_function(self, schema, data, context);

  RB_GC_GUARD(context_obj);

  return Qnil;
}

VALUE rb_valid(VALUE self, VALUE data) {
  CompiledSchema *schema = root_schema(self);
  Context *context;
  VALUE context_obj = create_context(&context);

  bool valid = is_valid(self, schema, data, context);

  RB_GC_GUARD(context_obj);

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
  Init_context();
  Init_is_valid();

  rb_define_method(schema_class, "compile", rb_compile, 0);
  rb_define_method(schema_class, "validate", rb_validate, 1);
  rb_define_method(schema_class, "valid?", rb_valid, 1);
}
