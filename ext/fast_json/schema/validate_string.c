#include "validate_string.h"
#include "error.h"

void validate_string(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!RB_TYPE_P(data, T_STRING))
    return yield_error(compiled_schema, data, context, "type_string");
}
