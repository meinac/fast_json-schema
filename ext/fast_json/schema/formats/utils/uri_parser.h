#ifndef FAST_JSON_FORMATS_UTILS_URI_PARSER_H
#define FAST_JSON_FORMATS_UTILS_URI_PARSER_H

#include <stdbool.h>

/*
* RFC 3986 URI / URI-reference parser, with optional RFC 3987 UTF-8 extensions.
*
* When require_scheme is true, input must start with a valid `scheme ":"`
* (RFC 3986 section 3.1). When false, input may also be a relative-ref including
* the empty string (RFC 3986 section 4.2 same-document reference).
*
* When allow_utf8 is true, any byte >= 0x80 must begin a valid UTF-8 sequence;
* such sequences are accepted anywhere `unreserved` characters are allowed
* (pragmatic IRI approach matching RFC 3987 section 2.2, without strict ucschar /
* iprivate Unicode range enforcement).
*/
bool parse_uri_reference(const char *s, long len, bool require_scheme, bool allow_utf8);

#endif
