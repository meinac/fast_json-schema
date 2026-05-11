#include "iri.h"
#include "formats/utils/uri_parser.h"
#include "error.h"

void validate_format_iri(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!parse_uri_reference(RSTRING_PTR(data), RSTRING_LEN(data), true, true))
    yield_error(compiled_schema, data, context, "format_iri");
}
