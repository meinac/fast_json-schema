#include "validate_number.h"
#include "error.h"

void validate_number(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!RB_INTEGER_TYPE_P(data) && !RB_TYPE_P(data, T_FLOAT))
    return yield_error(compiled_schema, data, context, "type_number");
}
