#include <ruby.h>
#include "types/compiled_schema.h"

bool parse_full_time(const char *s, long len);
void validate_format_time(VALUE, CompiledSchema *, VALUE, Context *);
