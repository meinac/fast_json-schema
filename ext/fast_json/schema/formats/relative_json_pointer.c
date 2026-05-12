#include "relative_json_pointer.h"
#include "formats/utils/json_pointer_parser.h"
#include "error.h"

#include <stdbool.h>

#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9')

/*
* draft-handrews-relative-json-pointer-02 section 3 Relative JSON Pointer parser.
*
*   relative-json-pointer = non-negative-integer ( "#" / json-pointer )
*   non-negative-integer  = "0" / non-zero-digit *DIGIT
*
* The integer prefix may not have leading zeros (e.g. "01" is invalid).
* The trailing portion may be "#" (key/index reference) or a valid JSON
* Pointer including the empty string ("0" alone is valid).
*/
static bool parse_relative_json_pointer(const char *s, long len) {
  if(len == 0) return false;

  long pos = 0;
  unsigned char first = (unsigned char)s[0];

  /*
  * Non-negative integer prefix
  */
  if(first == '0') {
    pos = 1;
  } else if(first >= '1' && first <= '9') {
    pos = 1;

    while(pos < len && IS_DIGIT((unsigned char)s[pos])) pos++;
  } else {
    return false;
  }

  /* 
  * Integer prefix only
  */
  if(pos == len) return true;

  /* 
  * "#" alone
  */
  if(s[pos] == '#') return pos + 1 == len;

  /* 
  * A valid JSON Pointer
  */
  return parse_json_pointer(s + pos, len - pos);
}

void validate_format_relative_json_pointer(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!parse_relative_json_pointer(RSTRING_PTR(data), RSTRING_LEN(data)))
    yield_error(compiled_schema, data, context, "format_relative_json_pointer");
}
