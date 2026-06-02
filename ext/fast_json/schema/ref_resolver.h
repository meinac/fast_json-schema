#include <ruby.h>
#include "compiled_schema.h"

void resolve_refs(CompiledSchema *compiled_schema, VALUE ref_data);
void register_path_for_ref_resolution(CompiledSchema *compiled_schema, VALUE ref_data);
void register_id_for_ref_resolution(CompiledSchema *compiled_schema, VALUE ref_data);
