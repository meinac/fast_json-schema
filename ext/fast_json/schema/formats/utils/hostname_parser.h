#ifndef FAST_JSON_FORMATS_UTILS_HOSTNAME_PARSER_H
#define FAST_JSON_FORMATS_UTILS_HOSTNAME_PARSER_H

#include <stdbool.h>

/*
* Hostname parser per RFC 1123 section 2.1 + RFC 1035 section 2.3.4 length limits.
*
* When allow_utf8 is true, label characters may also be valid UTF-8
* multi-byte sequences (pragmatic idn-hostname; not full IDNA2008).
*
* Returns true if the entire input is a syntactically valid hostname,
* false otherwise. A single trailing dot is allowed (RFC 1034 section 3.1).
*/
bool parse_hostname(const char *s, long len, bool allow_utf8);

#endif
