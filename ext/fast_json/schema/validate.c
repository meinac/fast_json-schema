#include "validate.h"
#include "error.h"

void no_op_validate(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  return;
}

void false_validate(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  yield_error(compiled_schema, data, context, "false_schema");
}

void validate(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  compiled_schema->type_validation_function(schema, compiled_schema, data, context);
}
