#include "formats/utils/ip_parser.h"

#include <stdbool.h>

#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9')
#define IS_HEX(c)   (IS_DIGIT(c) || ((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f'))

bool parse_ipv4(const char *s, long len) {
  long pos = 0;

  for(int i = 0; i < 4; i++) {
    if(pos >= len) return false;
    if(!IS_DIGIT(s[pos])) return false;

    /*
    * Reject leading zeros: "0" is allowed alone, but "01", "001" are not.
    * Per RFC 3986 section 3.2.2 dec-octet, leading zeros are forbidden to avoid
    * historical octal-vs-decimal ambiguity.
    */
    if(s[pos] == '0' && pos + 1 < len && IS_DIGIT(s[pos + 1])) return false;

    int value = 0;
    int digits = 0;

    while(pos < len && IS_DIGIT(s[pos]) && digits < 3) {
      value = value * 10 + (s[pos] - '0');
      pos++;
      digits++;
    }

    if(value > 255) return false;

    if(i < 3) {
      if(pos >= len || s[pos] != '.') return false;
      pos++;
    }
  }

  return pos == len;
}

/*
* Parse an IPv6 hex group: 1-4 hex digits.
* Returns the number of digits consumed, or 0 if none.
*/
static int parse_ipv6_group(const char *s, long len) {
  int digits = 0;

  while(digits < 4 && digits < len && IS_HEX((unsigned char)s[digits])) digits++;

  return digits;
}

bool parse_ipv6(const char *s, long len) {
  long pos = 0;
  int groups = 0;
  bool seen_compression = false;
  bool last_was_group = false;

  if(len >= 2 && s[0] == ':' && s[1] == ':') {
    seen_compression = true;
    pos = 2;
  } else if(len >= 1 && s[0] == ':') {
    return false;
  }

  while(pos < len) {
    /* Try IPv4 tail first when we are at the start of a group and there are dots ahead. */
    if(!last_was_group) {
      bool has_dot = false;

      for(long k = pos; k < len; k++) {
        if(s[k] == '.') { has_dot = true; break; }
        if(s[k] == ':') break;
      }

      if(has_dot) {
        if(!parse_ipv4(s + pos, len - pos)) return false;

        groups += 2;
        pos = len;
        last_was_group = true;
        break;
      }
    }

    int g = parse_ipv6_group(s + pos, len - pos);

    if(g == 0) return false;

    pos += g;
    groups++;
    last_was_group = true;

    if(pos == len) break;

    if(s[pos] != ':') return false;

    /* Possible "::" compression. */
    if(pos + 1 < len && s[pos + 1] == ':') {
      if(seen_compression) return false;

      seen_compression = true;
      pos += 2;
      last_was_group = false;

      if(pos == len) break; // trailing "::"
      continue;
    }

    pos++;
    last_was_group = false;
  }

  if(pos != len) return false;

  if(!last_was_group && !seen_compression) return false; // trailing single ":"

  if(seen_compression) {
    if(groups > 7) return false;

    return true;
  }

  return groups == 8;
}
