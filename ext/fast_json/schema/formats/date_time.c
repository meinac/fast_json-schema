#include "date_time.h"
#include "date.h"
#include "time.h"
#include "error.h"

/*
* Strict RFC 3339 date-time: full-date "T" full-time
* Only uppercase "T" separator is accepted.
*/
void validate_format_date_time(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  const char *s = RSTRING_PTR(data);
  long len = RSTRING_LEN(data);

  if(len < 12 || s[10] != 'T')
    return yield_error(compiled_schema, data, context, "format_date_time");

  if(!parse_full_date(s, 10))
    return yield_error(compiled_schema, data, context, "format_date_time");

  if(!parse_full_time(s + 11, len - 11))
    return yield_error(compiled_schema, data, context, "format_date_time");
}
