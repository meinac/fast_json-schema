#include "json_pointer.h"
#include "formats/utils/json_pointer_parser.h"
#include "error.h"

void validate_format_json_pointer(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!parse_json_pointer(RSTRING_PTR(data), RSTRING_LEN(data)))
    yield_error(compiled_schema, data, context, "format_json_pointer");
}
