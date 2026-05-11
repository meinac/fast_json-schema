#include "hostname.h"
#include "formats/utils/hostname_parser.h"
#include "error.h"

void validate_format_hostname(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!parse_hostname(RSTRING_PTR(data), RSTRING_LEN(data), false))
    yield_error(compiled_schema, data, context, "format_hostname");
}
