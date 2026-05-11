#ifndef FAST_JSON_FORMATS_UTILS_URI_TEMPLATE_PARSER_H
#define FAST_JSON_FORMATS_UTILS_URI_TEMPLATE_PARSER_H

#include <stdbool.h>

/*
* RFC 6570 URI Template parser.
*
* A URI Template is *( literals / expression ). Literals accept any valid
* UTF-8 (ucschar / iprivate) plus pct-encoded; expressions are ASCII-only:
*   "{" [ operator ] variable-list "}"
*
* Returns true if the entire input is a well-formed URI Template. The empty
* string is valid (zero repetitions of literals/expression).
*/
bool parse_uri_template(const char *s, long len);

#endif
