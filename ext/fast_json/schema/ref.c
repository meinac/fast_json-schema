#include "ref.h"

void validate_ref(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  CompiledSchema *ref_schema = compiled_schema->ref_schema;

  ref_schema->validation_function(schema, ref_schema, data, context);
}
