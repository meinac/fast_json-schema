#include "validate.h"
#include "error.h"

void no_op_validate(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  return;
}

void false_validate(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  yield_error(compiled_schema, data, context, "false");
}

void validate(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  return; // TODO
}
