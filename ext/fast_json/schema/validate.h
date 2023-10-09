#include <ruby.h>
#include "compiled_schema.h"

void no_op_validate(VALUE, CompiledSchema *, VALUE, Context *);
void false_validate(VALUE, CompiledSchema *, VALUE, Context *);
void validate(VALUE, CompiledSchema *, VALUE, Context *);
