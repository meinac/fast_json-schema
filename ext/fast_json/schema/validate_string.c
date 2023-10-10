#include "validate_string.h"
#include "error.h"

void validate_string(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!RB_TYPE_P(data, T_STRING))
    return yield_error(compiled_schema, data, context, "type_string");

  if(compiled_schema->maxLength_val != Qundef) {
    if(RSTRING_LEN(data) > NUM2LONG(compiled_schema->maxLength_val))
      yield_error(compiled_schema, data, context, "maxLength");
  }

  if(compiled_schema->minLength_val != Qundef) {
    if(RSTRING_LEN(data) < NUM2LONG(compiled_schema->minLength_val))
      yield_error(compiled_schema, data, context, "minLength");
  }

  if(compiled_schema->pattern_val != Qundef) {
    VALUE regexp = rb_reg_new_str(compiled_schema->pattern_val, 0);
    VALUE result = rb_reg_match(regexp, data);

    if(NIL_P(result))
      yield_error(compiled_schema, data, context, "pattern");
  }
}
