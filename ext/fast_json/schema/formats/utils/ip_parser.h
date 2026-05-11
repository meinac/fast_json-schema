#ifndef FAST_JSON_FORMATS_UTILS_IP_PARSER_H
#define FAST_JSON_FORMATS_UTILS_IP_PARSER_H

#include <stdbool.h>

/*
* IPv4 dotted-decimal address per RFC 2673 section 3.2 / RFC 3986 section 3.2.2:
*   IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet
*   dec-octet   = 1*3DIGIT, value 0..255, no leading zeros
*
* Returns true if the entire input is a valid IPv4 address, false otherwise.
*/
bool parse_ipv4(const char *s, long len);

/*
* IPv6 address per RFC 4291 section 2.2:
*   - Up to 8 groups of 1-4 hex digits separated by ":"
*   - At most one "::" compression standing for one or more zero groups
*   - Optional trailing IPv4 dotted form (counts as 2 groups)
*
* Returns true if the entire input is a valid IPv6 address, false otherwise.
*/
bool parse_ipv6(const char *s, long len);

#endif
