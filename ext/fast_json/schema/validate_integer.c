#include "validate_integer.h"
#include "error.h"

void validate_integer(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!RB_INTEGER_TYPE_P(data))
    return yield_error(compiled_schema, data, context, "type_integer");
}
