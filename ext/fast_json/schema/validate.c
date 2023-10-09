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

void validate_null(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!NIL_P(data))
    yield_error(compiled_schema, data, context, "type_null");
}

void validate_bool(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(data != Qtrue && data != Qfalse)
    yield_error(compiled_schema, data, context, "type_bool");
}

void validate_string(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!RB_TYPE_P(data, T_STRING))
    return yield_error(compiled_schema, data, context, "type_string");
}

void validate_integer(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!RB_INTEGER_TYPE_P(data))
    return yield_error(compiled_schema, data, context, "type_integer");
}

void validate_number(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!RB_INTEGER_TYPE_P(data) && !RB_TYPE_P(data, T_FLOAT))
    return yield_error(compiled_schema, data, context, "type_number");
}

void validate_array(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!RB_TYPE_P(data, T_ARRAY))
    return yield_error(compiled_schema, data, context, "type_array");
}

void validate_object(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!RB_TYPE_P(data, T_HASH))
    return yield_error(compiled_schema, data, context, "type_object");
}
