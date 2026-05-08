#include <ruby.h>
#include "types/compiled_schema.h"

bool parse_full_date(const char *s, long len);
void validate_format_date(VALUE, CompiledSchema *, VALUE, Context *);
