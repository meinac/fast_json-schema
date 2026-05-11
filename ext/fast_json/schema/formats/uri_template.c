#include "uri_template.h"
#include "formats/utils/uri_template_parser.h"
#include "error.h"

void validate_format_uri_template(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!parse_uri_template(RSTRING_PTR(data), RSTRING_LEN(data)))
    yield_error(compiled_schema, data, context, "format_uri_template");
}
