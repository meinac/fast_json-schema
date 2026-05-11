#include "idn_email.h"
#include "formats/utils/addr_spec_parser.h"
#include "error.h"

void validate_format_idn_email(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!parse_addr_spec(RSTRING_PTR(data), RSTRING_LEN(data), true))
    yield_error(compiled_schema, data, context, "format_idn_email");
}
