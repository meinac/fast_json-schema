#include <ruby.h>
#include "keywords.h"
#include "compiled_schema.h"
#include "types/context.h"
#include "error.h"
#include "value_pointer_caster.h"

VALUE rb_validate_with_schema(VALUE self, VALUE compiled_schema_obj, VALUE data, VALUE context_pointer) {
  if (!rb_block_given_p()) {
    VALUE args[3] = { compiled_schema_obj, data, context_pointer};

    RETURN_SIZED_ENUMERATOR(self, 3, args, 0);
  }

  CompiledSchema *compiled_schema;
  GetCompiledSchema(compiled_schema_obj, compiled_schema);

  Context context_s;
  Context *context;

  /*
  * TODO:
  * I am highly concerned about the security of this hack as users can provide
  * integer values to this method which can point to a random place in memory.
  * Maybe I should return back to using Ruby typed Context struct.
  * I will sleep on this idea.
  */
  if(NIL_P(context_pointer)) {
    context_s.path[0] = root_path_str;
    context_s.depth = 0;
    context_s.jump = false;

    context = &context_s;
  } else {
    context = NUM2PTR(context_pointer);
  }

  compiled_schema->validation_function(self, compiled_schema, data, context);

  return Qnil;
}

VALUE rb_validate(VALUE self, VALUE data) {
  VALUE compiled_schema_obj = rb_ivar_get(self, rb_intern("compiled_schema"));

  return rb_funcall(self, rb_intern("validate_with_schema"), 3, compiled_schema_obj, data, Qnil);
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
