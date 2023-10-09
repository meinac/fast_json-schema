#include <ruby.h>
#include "compiled_schema.h"

void yield_error(CompiledSchema *, VALUE, Context *, const char *);
void Init_error(VALUE);
