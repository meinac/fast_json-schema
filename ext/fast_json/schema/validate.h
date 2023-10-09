#include <ruby.h>
#include "compiled_schema.h"

void no_op_validate(VALUE, CompiledSchema *, VALUE, Context *);
void false_validate(VALUE, CompiledSchema *, VALUE, Context *);
void validate(VALUE, CompiledSchema *, VALUE, Context *);

void validate_null(VALUE, CompiledSchema *, VALUE, Context *);
void validate_bool(VALUE, CompiledSchema *, VALUE, Context *);
void validate_string(VALUE, CompiledSchema *, VALUE, Context *);
void validate_integer(VALUE, CompiledSchema *, VALUE, Context *);
void validate_number(VALUE, CompiledSchema *, VALUE, Context *);
void validate_array(VALUE, CompiledSchema *, VALUE, Context *);
void validate_object(VALUE, CompiledSchema *, VALUE, Context *);
