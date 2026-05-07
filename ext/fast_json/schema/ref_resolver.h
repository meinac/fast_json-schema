#include <ruby.h>
#include "compiled_schema.h"

void resolve_refs(CompiledSchema *compiled_schema, VALUE ref_data);
void register_schema_for_ref_resolution(CompiledSchema *compiled_schema, VALUE ref_data);