#include <ruby.h>
#include "keywords.h"
#include "compiled_schema.h"
#include "context.h"
#include "error.h"

VALUE rb_validate_with_schema(VALUE self, VALUE compiled_schema_obj, VALUE data, VALUE context_object) {
  if (!rb_block_given_p()) {
    VALUE args[3] = { compiled_schema_obj, data, context_object};

    RETURN_SIZED_ENUMERATOR(self, 3, args, 0);
  }

  CompiledSchema *compiled_schema;
  GetCompiledSchema(compiled_schema_obj, compiled_schema);

  Context *context;
  GetContext(context_object, context);

  compiled_schema->validate_function(self, compiled_schema, data, context);

  return Qnil;
}

VALUE rb_validate(VALUE self, VALUE data) {
  VALUE compiled_schema_obj = rb_ivar_get(self, rb_intern("compiled_schema"));
  VALUE context_object = create_context_object(root_path_str);

  return rb_funcall(self, rb_intern("validate_with_schema"), 3, compiled_schema_obj, data, context_object);
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
  rb_define_private_method(schema_class, "validate_with_schema", rb_validate_with_schema, 3);
}
