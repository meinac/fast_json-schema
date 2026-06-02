#include <ruby.h>
#include "compiled_schema.h"
#include "types/compile_context.h"

void compile_nested_schemas(CompiledSchema *, VALUE, CompileContext *);
