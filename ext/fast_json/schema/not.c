#include "not.h"
#include "error.h"

extern bool is_valid(VALUE, CompiledSchema *, VALUE, Context *);

void validate_not(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  bool valid = is_valid(schema, compiled_schema->not_schema, data, context);

  if(valid)
    yield_error(compiled_schema->not_schema, data, context, "not");
}
