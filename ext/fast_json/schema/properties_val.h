#include <ruby.h>
#include "types/compiled_schema.h"
#include "types/compile_context.h"

void compile_properties_val(CompiledSchema *, VALUE, CompileContext *);
void compile_pattern_properties_val(CompiledSchema *, VALUE, CompileContext *);
void compile_dependencies_val(CompiledSchema *, VALUE, CompileContext *);
