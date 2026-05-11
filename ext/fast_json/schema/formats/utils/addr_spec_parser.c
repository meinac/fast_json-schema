#include "formats/utils/addr_spec_parser.h"
#include "formats/utils/utf8.h"

#include <stdbool.h>
#include <string.h>

/*
* RFC 5321 length limits.
*/
#define MAX_LOCAL_PART  64
#define MAX_DOMAIN     255

#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9')
#define IS_ALPHA(c) (((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))
#define IS_HEX(c)   (IS_DIGIT(c) || ((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f'))

/*
* atext per RFC 5322:
*   ALPHA / DIGIT / "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" /
*   "/" / "=" / "?" / "^" / "_" / "`" / "{" / "|" / "}" / "~"
*/
static bool is_atext(unsigned char c) {
  if(IS_ALPHA(c) || IS_DIGIT(c)) return true;

  switch(c) {
    case '!': case '#': case '$': case '%': case '&': case '\'':
    case '*': case '+': case '-': case '/': case '=': case '?':
    case '^': case '_': case '`': case '{': case '|': case '}':
    case '~':
      return true;
  }

  return false;
}

/*
* qtext per RFC 5322:
*   %d33 / %d35-91 / %d93-126
* (i.e. VCHAR except '"' and '\').
*/
static bool is_qtext(unsigned char c) {
  return c == 33 || (c >= 35 && c <= 91) || (c >= 93 && c <= 126);
}

/*
* General-address-literal dcontent per RFC 5321 §4.1.3:
*   dcontent = %d33-90 / %d94-126
*/
static bool is_dcontent(unsigned char c) {
  return (c >= 33 && c <= 90) || (c >= 94 && c <= 126);
}

/*
* VCHAR per RFC 5234: %x21-7E.
* WSP per RFC 5234:   SP / HTAB.
*/
static bool is_vchar(unsigned char c) { return c >= 0x21 && c <= 0x7E; }
static bool is_wsp(unsigned char c)   { return c == ' ' || c == '\t'; }

/*
* Try to consume a single atext character (or a valid UTF-8 sequence when
* allow_utf8 is true). Returns the number of bytes consumed, or 0 if the
* input does not start with an atext character.
*/
static long consume_atext(const char *s, long len, bool allow_utf8) {
  unsigned char c = (unsigned char)s[0];

  if(c < 0x80) return is_atext(c) ? 1 : 0;
  if(!allow_utf8) return 0;

  return utf8_seq_len((const unsigned char *)s, len);
}

/*
* Parse 1*atext starting at s. Returns the number of bytes consumed, or 0 if
* there is not at least one atext character.
*/
static long parse_atext_run(const char *s, long len, bool allow_utf8) {
  long pos = 0;

  while(pos < len) {
    long n = consume_atext(s + pos, len - pos, allow_utf8);

    if(n == 0) break;

    pos += n;
  }

  return pos;
}

/*
* Parse a dot-atom-text: 1*atext *("." 1*atext).
* Returns the number of bytes consumed, or 0 on failure.
*/
static long parse_dot_atom(const char *s, long len, bool allow_utf8, bool allow_trailing_dot) {
  long first = parse_atext_run(s, len, allow_utf8);

  if(first == 0) return 0;

  long pos = first;

  while(pos < len && s[pos] == '.') {
    long next = parse_atext_run(s + pos + 1, len - pos - 1, allow_utf8);

    if(next == 0) {
      /*
      * A single trailing dot is accepted in the domain to support fully
      * qualified DNS names; consecutive dots and trailing dots in the
      * local-part remain rejected.
      */
      if(allow_trailing_dot && pos + 1 == len) return len;

      return 0;
    }

    pos += 1 + next;
  }

  return pos;
}

/*
* Parse a quoted-string (excluding any surrounding CFWS - we don't allow it):
*   DQUOTE *( qtext / quoted-pair ) DQUOTE
*   quoted-pair = "\" ( VCHAR / WSP )
*
* WSP between qcontent items is allowed by RFC 5322 (FWS), but we keep it
* strict: WSP is only allowed inside the quoted-string as bare octets if it
* is part of qtext. SP (0x20) and HTAB (0x09) are not in qtext, but are
* commonly accepted; for strictness we accept SP/HTAB as a bare octet inside
* the quoted-string (as if folded FWS) - this mirrors the RFC's qcontent
* allowance via FWS without permitting line folding.
*
* Returns the number of bytes consumed including the surrounding DQUOTEs,
* or 0 on failure.
*/
static long parse_quoted_string(const char *s, long len, bool allow_utf8) {
  if(len < 2 || s[0] != '"') return 0;

  long pos = 1;

  while(pos < len && s[pos] != '"') {
    unsigned char c = (unsigned char)s[pos];

    if(c == '\\') {
      if(pos + 1 >= len) return 0;

      unsigned char next = (unsigned char)s[pos + 1];

      if(!is_vchar(next) && !is_wsp(next)) return 0;

      pos += 2;
      continue;
    }

    if(is_qtext(c) || is_wsp(c)) {
      pos++;
      continue;
    }

    if(allow_utf8 && c >= 0x80) {
      long n = utf8_seq_len((const unsigned char *)s + pos, len - pos);

      if(n == 0) return 0;

      pos += n;
      continue;
    }

    return 0;
  }

  if(pos >= len || s[pos] != '"') return 0;

  return pos + 1;
}

/*
* Parse the local-part: dot-atom-text or quoted-string.
* Returns the number of bytes consumed, or 0 on failure.
*/
static long parse_local_part(const char *s, long len, bool allow_utf8) {
  if(len == 0) return 0;

  if(s[0] == '"') return parse_quoted_string(s, len, allow_utf8);

  return parse_dot_atom(s, len, allow_utf8, false);
}

/*
* IPv4-address-literal per RFC 5321:
*   Snum "." Snum "." Snum "." Snum
*   Snum = 1*3DIGIT, value 0..255
*/
static bool parse_ipv4_literal(const char *s, long len) {
  long pos = 0;

  for(int i = 0; i < 4; i++) {
    if(pos >= len) return false;
    if(!IS_DIGIT(s[pos])) return false;

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

/*
* IPv6-address-literal per RFC 4291 / RFC 5321 §4.1.3:
*   IPv6-addr =  IPv6-full / IPv6-comp / IPv6v4-full / IPv6v4-comp
*
* Rules implemented:
*   - Up to 8 groups of 1-4 hex digits separated by ":"
*   - At most one "::" compression that may stand for one or more zero groups
*   - Optional trailing IPv4 dotted form, which counts as 2 groups
*/
static bool parse_ipv6_addr(const char *s, long len) {
  long pos = 0;
  int groups = 0;
  bool seen_compression = false;
  int groups_before_compression = 0;
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
        if(!parse_ipv4_literal(s + pos, len - pos)) return false;

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
      groups_before_compression = groups;
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

    (void)groups_before_compression;
    return true;
  }

  return groups == 8;
}

/*
* IPv6-address-literal: "IPv6:" IPv6-addr
*/
static bool parse_ipv6_literal(const char *s, long len) {
  if(len < 5) return false;
  if(memcmp(s, "IPv6:", 5) != 0) return false;

  return parse_ipv6_addr(s + 5, len - 5);
}

/*
* Ldh-str per RFC 5321:
*   Let-dig *( *("-") Let-dig )
* simplified to: ALPHA/DIGIT, may contain '-' but must end with ALPHA/DIGIT.
*/
static bool parse_ldh_str(const char *s, long len) {
  if(len == 0) return false;
  if(!IS_ALPHA((unsigned char)s[0]) && !IS_DIGIT((unsigned char)s[0])) return false;

  for(long i = 0; i < len; i++) {
    unsigned char c = (unsigned char)s[i];

    if(!IS_ALPHA(c) && !IS_DIGIT(c) && c != '-') return false;
  }

  if(s[len - 1] == '-') return false;

  return true;
}

/*
* General-address-literal per RFC 5321 §4.1.3:
*   Standardized-tag ":" 1*dcontent
*   Standardized-tag = Ldh-str
*/
static bool parse_general_address_literal(const char *s, long len, bool allow_utf8) {
  long colon = -1;

  for(long i = 0; i < len; i++) {
    if(s[i] == ':') { colon = i; break; }
  }

  if(colon <= 0 || colon >= len - 1) return false;

  if(!parse_ldh_str(s, colon)) return false;

  /*
  * Standardized-tag must be a registered token; reject "IPv6" here so that an
  * "IPv6:..." body that failed parse_ipv6_literal does not silently match the
  * General-address-literal form. Also require at least one ALPHA in the tag
  * to avoid all-digit tags accidentally matching IPv6-shaped content.
  */
  if(colon == 4 && memcmp(s, "IPv6", 4) == 0) return false;

  bool has_alpha = false;
  for(long i = 0; i < colon; i++) {
    if(IS_ALPHA((unsigned char)s[i])) { has_alpha = true; break; }
  }
  if(!has_alpha) return false;

  long pos = colon + 1;

  if(pos >= len) return false;

  while(pos < len) {
    unsigned char c = (unsigned char)s[pos];

    if(c < 0x80) {
      if(!is_dcontent(c)) return false;
      pos++;
      continue;
    }

    if(!allow_utf8) return false;

    long n = utf8_seq_len((const unsigned char *)s + pos, len - pos);

    if(n == 0) return false;

    pos += n;
  }

  return true;
}

/*
* domain-literal contents (between "[" and "]"). Tries IPv4, then IPv6, then
* General-address-literal. Per RFC 6531 only General-address-literal admits
* UTF-8 in dcontent; IPv4 and IPv6 literals remain ASCII.
*/
static bool parse_domain_literal_body(const char *s, long len, bool allow_utf8) {
  if(parse_ipv4_literal(s, len)) return true;
  if(parse_ipv6_literal(s, len)) return true;

  return parse_general_address_literal(s, len, allow_utf8);
}

/*
* Parse a domain-literal: "[" body "]"
* Returns the number of bytes consumed, or 0 on failure.
*/
static long parse_domain_literal(const char *s, long len, bool allow_utf8) {
  if(len < 2 || s[0] != '[') return 0;

  long end = -1;

  for(long i = 1; i < len; i++) {
    if(s[i] == ']') { end = i; break; }
  }

  if(end == -1) return 0;

  if(!parse_domain_literal_body(s + 1, end - 1, allow_utf8)) return 0;

  return end + 1;
}

/*
* Parse the domain: dot-atom-text or domain-literal.
* Returns the number of bytes consumed, or 0 on failure.
*/
static long parse_domain(const char *s, long len, bool allow_utf8) {
  if(len == 0) return 0;

  if(s[0] == '[') return parse_domain_literal(s, len, allow_utf8);

  return parse_dot_atom(s, len, allow_utf8, true);
}

bool parse_addr_spec(const char *s, long len, bool allow_utf8) {
  if(len == 0) return false;

  long local_len = parse_local_part(s, len, allow_utf8);

  if(local_len == 0) return false;
  if(local_len > MAX_LOCAL_PART) return false;
  if(local_len >= len) return false;
  if(s[local_len] != '@') return false;

  long domain_off = local_len + 1;
  long domain_len = len - domain_off;

  if(domain_len <= 0) return false;
  if(domain_len > MAX_DOMAIN) return false;

  long consumed = parse_domain(s + domain_off, domain_len, allow_utf8);

  if(consumed == 0) return false;
  if(consumed != domain_len) return false;

  return true;
}
