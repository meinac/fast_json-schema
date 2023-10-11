#include "validate_array.h"
#include "error.h"
#include "value_pointer_caster.h"

static void validate_items(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  long i;

  context->depth++;

  for(i = 0; i < RARRAY_LEN(data); i++) {
    context->path[context->depth] = LONG2NUM(i);

    compiled_schema->validation_function(schema, compiled_schema->items_schema, rb_ary_entry(data, i), context);
  }

  context->depth--;
}

static void validate_contains(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  long i, valid_count = 0;

  VALUE compiled_schema_obj = WrapCompiledSchema(compiled_schema->contains_schema);

  for(i = 0; i < RARRAY_LEN(data); i++) {
    VALUE entry = rb_ary_entry(data, i);
    VALUE enumerator = rb_funcall(schema, rb_intern("validate_with_schema"), 3, compiled_schema_obj, entry, PTR2NUM(context));
    VALUE none = rb_funcall(enumerator, rb_intern("none?"), 0);

    if(none == Qtrue)
      valid_count++;
  }

  if(valid_count == 0)
    yield_error(compiled_schema->contains_schema, data, context, "contains");

  if(RB_INTEGER_TYPE_P(compiled_schema->maxContains_val)) {
    if(valid_count > NUM2LONG(compiled_schema->maxContains_val))
      yield_error(compiled_schema, data, context, "maxContains");
  }
}

void validate_array(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!RB_TYPE_P(data, T_ARRAY))
    return yield_error(compiled_schema, data, context, "type_array");

  if(compiled_schema->maxItems_val != Qundef) {
    if(RARRAY_LEN(data) > NUM2LONG(compiled_schema->maxItems_val))
      yield_error(compiled_schema, data, context, "maxItems");
  }

  if(compiled_schema->minItems_val != Qundef) {
    if(RARRAY_LEN(data) < NUM2LONG(compiled_schema->minItems_val))
      yield_error(compiled_schema, data, context, "minItems");
  }

  if(compiled_schema->uniqueItems_val == Qtrue) {
    // I don't really want to deal with comparing array elements
    VALUE unique_arr = rb_funcall(data, rb_intern("uniq"), 0);

    if(RARRAY_LEN(unique_arr) < RARRAY_LEN(data))
      yield_error(compiled_schema, data, context, "uniqueItems");
  }

  if(compiled_schema->items_schema != NULL)
    validate_items(schema, compiled_schema, data, context);

  if(compiled_schema->contains_schema != NULL)
    validate_contains(schema, compiled_schema, data, context);
}
