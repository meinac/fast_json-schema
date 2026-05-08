#include "time.h"
#include "error.h"

#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9')

/*
* Strict RFC 3339 full-time parser: HH:MM:SS[.fraction](Z|(+|-)HH:MM)
* Returns true if valid, false otherwise.
*/
bool parse_full_time(const char *s, long len) {
  if(len < 9) return false;

  if(!IS_DIGIT(s[0]) || !IS_DIGIT(s[1])) return false;
  if(s[2] != ':') return false;
  if(!IS_DIGIT(s[3]) || !IS_DIGIT(s[4])) return false;
  if(s[5] != ':') return false;
  if(!IS_DIGIT(s[6]) || !IS_DIGIT(s[7])) return false;

  int hour   = (s[0] - '0') * 10 + (s[1] - '0');
  int minute = (s[3] - '0') * 10 + (s[4] - '0');
  int second = (s[6] - '0') * 10 + (s[7] - '0');

  if(hour > 23) return false;
  if(minute > 59) return false;
  if(second > 60) return false; // 60 is allowed for leap second per RFC 3339

  long pos = 8;

  /*
  * Fractional part calculation.
  * Fractional part is optional(See RFC 3339 for more details).
  */
  if(s[pos] == '.') {
    pos++;

    long fraction_start = pos;

    while(pos < len && IS_DIGIT(s[pos])) pos++;

    if(pos == fraction_start) return false; // empty fractional part(`12:34:56.`)
  }

  if(pos >= len) return false;

  if(s[pos] == 'Z') return pos == len - 1;

  if(s[pos] != '+' && s[pos] != '-') return false;
  if(len - pos != 6) return false;
  if(!IS_DIGIT(s[pos + 1]) || !IS_DIGIT(s[pos + 2])) return false;
  if(s[pos + 3] != ':') return false;
  if(!IS_DIGIT(s[pos + 4]) || !IS_DIGIT(s[pos + 5])) return false;

  int offset_hour   = (s[pos + 1] - '0') * 10 + (s[pos + 2] - '0');
  int offset_minute = (s[pos + 4] - '0') * 10 + (s[pos + 5] - '0');

  if(offset_hour > 23) return false;
  if(offset_minute > 59) return false;

  return true;
}

void validate_format_time(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!parse_full_time(RSTRING_PTR(data), RSTRING_LEN(data)))
    yield_error(compiled_schema, data, context, "format_time");
}
