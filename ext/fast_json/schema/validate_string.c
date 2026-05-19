#include "validate_string.h"
#include "error.h"
#include "formats/format.h"

void validate_string(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!RB_TYPE_P(data, T_STRING))
    return yield_error(compiled_schema, data, context, "type_string");

  if(compiled_schema->maxLength_val != Qundef) {
    VALUE string_length = rb_str_length(data);

    if(NUM2LONG(string_length) > NUM2LONG(compiled_schema->maxLength_val))
      yield_error(compiled_schema, data, context, "maxLength");
  }

  if(compiled_schema->minLength_val != Qundef) {
    VALUE string_length = rb_str_length(data);

    if(NUM2LONG(string_length) < NUM2LONG(compiled_schema->minLength_val))
      yield_error(compiled_schema, data, context, "minLength");
  }

  if(compiled_schema->pattern_val != Qundef) {
    VALUE regexp = rb_reg_new_str(compiled_schema->pattern_val, 0);
    VALUE result = rb_reg_match(regexp, data);

    if(NIL_P(result))
      yield_error(compiled_schema, data, context, "pattern");
  }

  compiled_schema->format_validation_function(schema, compiled_schema, data, context);
}
