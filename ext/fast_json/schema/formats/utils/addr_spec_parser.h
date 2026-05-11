#ifndef FAST_JSON_FORMATS_UTILS_ADDR_SPEC_PARSER_H
#define FAST_JSON_FORMATS_UTILS_ADDR_SPEC_PARSER_H

#include <stdbool.h>

/*
* Strict RFC 5322 addr-spec parser (with RFC 5321 address-literals).
* When allow_utf8 is true, RFC 6531 UTF8-non-ASCII extensions are accepted in
* atext, qtext and General-address-literal dcontent productions.
*
* Returns true if the entire input is a valid addr-spec, false otherwise.
* No CFWS, no obs-* productions, no leading/trailing whitespace.
*/
bool parse_addr_spec(const char *s, long len, bool allow_utf8);

#endif
