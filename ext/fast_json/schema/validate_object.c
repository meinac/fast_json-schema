#include "validate_object.h"
#include "error.h"

static void validate_required(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  long i;
  VALUE required_val = compiled_schema->required_val;

  for(i = 0; i < RARRAY_LEN(required_val); i++) {
    VALUE entry = rb_ary_entry(required_val, i);

    if(rb_funcall(data, rb_intern("key?"), 1, entry) == Qfalse)
      yield_error(compiled_schema, data, context, "required");
  }
}

void validate_object(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!RB_TYPE_P(data, T_HASH))
    return yield_error(compiled_schema, data, context, "type_object");

  if(RB_INTEGER_TYPE_P(compiled_schema->maxProperties_val)) {
    if(RHASH_SIZE(data) > NUM2ULONG(compiled_schema->maxProperties_val))
      yield_error(compiled_schema, data, context, "maxProperties");
  }

  if(RB_INTEGER_TYPE_P(compiled_schema->minProperties_val)) {
    if(RHASH_SIZE(data) < NUM2ULONG(compiled_schema->minProperties_val))
      yield_error(compiled_schema, data, context, "minProperties");
  }

  if(compiled_schema->required_val != Qundef)
    validate_required(schema, compiled_schema, data, context);
}
