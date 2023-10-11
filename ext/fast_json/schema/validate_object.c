#include "validate_object.h"
#include "error.h"

void validate_object(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!RB_TYPE_P(data, T_HASH))
    return yield_error(compiled_schema, data, context, "type_object");

  if(RB_INTEGER_TYPE_P(compiled_schema->maxProperties_val)) {
    if(RHASH_SIZE(data) > NUM2ULONG(compiled_schema->maxProperties_val))
      yield_error(compiled_schema, data, context, "maxProperties");
  }
}
