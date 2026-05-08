#include <ruby.h>
#include "types/compiled_schema.h"

void no_op_format_validate(VALUE, CompiledSchema *, VALUE, Context *);
format_validation_function format_validation_function_for(VALUE format_val);
