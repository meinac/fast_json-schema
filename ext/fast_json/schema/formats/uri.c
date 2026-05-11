#include "uri.h"
#include "formats/utils/uri_parser.h"
#include "error.h"

void validate_format_uri(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!parse_uri_reference(RSTRING_PTR(data), RSTRING_LEN(data), true, false))
    yield_error(compiled_schema, data, context, "format_uri");
}
