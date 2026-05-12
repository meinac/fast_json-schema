#ifndef FAST_JSON_FORMATS_UTILS_JSON_POINTER_PARSER_H
#define FAST_JSON_FORMATS_UTILS_JSON_POINTER_PARSER_H

#include <stdbool.h>

/*
* RFC 6901 section 3 JSON Pointer parser.
*
*   json-pointer    = *( "/" reference-token )
*   reference-token = *( unescaped / escaped )
*   unescaped       = %x00-2E / %x30-7D / %x7F-10FFFF
*   escaped         = "~" ( "0" / "1" )
*
* The empty string is a valid JSON Pointer (whole-document reference).
* UTF-8 multi-byte sequences are accepted in token content per the
* unescaped %x80-10FFFF range. NUL bytes are accepted per %x00-2E.
*/
bool parse_json_pointer(const char *s, long len);

#endif
