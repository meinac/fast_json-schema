#include "ipv4.h"
#include "formats/utils/ip_parser.h"
#include "error.h"

void validate_format_ipv4(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!parse_ipv4(RSTRING_PTR(data), RSTRING_LEN(data)))
    yield_error(compiled_schema, data, context, "format_ipv4");
}
