#include "if.h"
#include "error.h"

extern bool is_valid(VALUE, CompiledSchema *, VALUE, Context *);

static void run_then(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  bool valid = is_valid(schema, compiled_schema, data, context);

  if(!valid)
    yield_error(compiled_schema, data, context, "if/then");
}

static void run_else(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  bool valid = is_valid(schema, compiled_schema, data, context);

  if(!valid)
    yield_error(compiled_schema, data, context, "if/else");
}

void validate_if(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(compiled_schema->then_schema == NULL && compiled_schema->else_schema == NULL)
    return;

  bool valid = is_valid(schema, compiled_schema->if_schema, data, context);

  if(valid && compiled_schema->then_schema != NULL) {
    run_then(schema, compiled_schema->then_schema, data, context);
  } else if(!valid && compiled_schema->else_schema != NULL) {
    run_else(schema, compiled_schema->else_schema, data, context);
  }
}
