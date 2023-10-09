#include "validate_null.h"
#include "error.h"

void validate_null(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!NIL_P(data))
    yield_error(compiled_schema, data, context, "type_null");
}
