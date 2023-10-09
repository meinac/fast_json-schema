#include "validate_bool.h"
#include "error.h"

void validate_bool(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(data != Qtrue && data != Qfalse)
    yield_error(compiled_schema, data, context, "type_bool");
}
