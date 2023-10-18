#include "all_of.h"
#include "error.h"

extern bool is_valid(VALUE, CompiledSchema *, VALUE, Context *);

void validate_all_of(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  long i, valid_count = 0;

  for(i = 0; i < RARRAY_LEN(compiled_schema->allOf_val); i++) {
    VALUE child_compiled_schema_obj = rb_ary_entry(compiled_schema->allOf_val, i);

    CompiledSchema *child_compiled_schema;
    GetCompiledSchema(child_compiled_schema_obj, child_compiled_schema);

    bool valid = is_valid(schema, child_compiled_schema, data, context);

    if(valid)
      valid_count++;
  }

  if(valid_count != RARRAY_LEN(compiled_schema->allOf_val))
    yield_error(compiled_schema, data, context, "allOf");
}
