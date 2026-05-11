#include "formats/utils/uri_template_parser.h"
#include "formats/utils/utf8.h"

#include <stdbool.h>

#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9')
#define IS_ALPHA(c) (((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))
#define IS_HEX(c)   (IS_DIGIT(c) || ((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f'))

/*
* varchar per RFC 6570 section 2.3:
*   ALPHA / DIGIT / "_"
* (pct-encoded handled separately).
*/
static bool is_varchar_ascii(unsigned char c) {
  return IS_ALPHA(c) || IS_DIGIT(c) || c == '_';
}

/*
* Operator per RFC 6570 section 2.2:
*   op-level2  = "+" / "#"
*   op-level3  = "." / "/" / ";" / "?" / "&"
*   op-reserve = "=" / "," / "!" / "@" / "|"
*/
static bool is_operator(unsigned char c) {
  switch(c) {
    case '+': case '#':
    case '.': case '/': case ';': case '?': case '&':
    case '=': case ',': case '!': case '@': case '|':
      return true;
  }
  return false;
}

/*
* Literal ASCII per RFC 6570 section 2.1:
*   %x21 / %x23-24 / %x26 / %x28-3B / %x3D / %x3F-5B / %x5D / %x5F / %x61-7A / %x7E
*
* That is: any printable ASCII except SP, control chars, DEL, and the set
* { '"' '%' '\'' '<' '>' '\\' '^' '`' '{' '|' '}' }.
* ('%' is allowed only as the start of a pct-encoded triple.)
*/
static bool is_literal_ascii(unsigned char c) {
  if(c == 0x21) return true;                       // "!"
  if(c >= 0x23 && c <= 0x24) return true;          // "#" "$"
  if(c == 0x26) return true;                       // "&"
  if(c >= 0x28 && c <= 0x3B) return true;          // "(" .. ";"
  if(c == 0x3D) return true;                       // "="
  if(c >= 0x3F && c <= 0x5B) return true;          // "?" .. "["
  if(c == 0x5D) return true;                       // "]"
  if(c == 0x5F) return true;                       // "_"
  if(c >= 0x61 && c <= 0x7A) return true;          // "a" .. "z"
  if(c == 0x7E) return true;                       // "~"
  return false;
}

/*
* pct-encoded per RFC 3986 section 2.1: "%" HEXDIG HEXDIG.
* Returns 3 on success, 0 on failure.
*/
static long consume_pct_encoded(const char *s, long len) {
  if(len < 3 || s[0] != '%') return 0;
  if(!IS_HEX((unsigned char)s[1]) || !IS_HEX((unsigned char)s[2])) return 0;
  return 3;
}

/*
* Consume one literal unit: literal ASCII byte, pct-encoded triple, or
* valid UTF-8 multi-byte sequence. Returns bytes consumed, or 0 on failure.
*/
static long consume_literal_byte(const char *s, long len) {
  unsigned char c = (unsigned char)s[0];

  if(c < 0x80) {
    if(c == '%') return consume_pct_encoded(s, len);
    return is_literal_ascii(c) ? 1 : 0;
  }

  long n = utf8_seq_len((const unsigned char *)s, len);
  return n >= 2 ? n : 0;
}

/*
* Consume a run of literals (1+ bytes) until "{" or an invalid byte.
* Returns bytes consumed (must be > 0).
*/
static long parse_literals_run(const char *s, long len) {
  long pos = 0;
  while(pos < len && s[pos] != '{') {
    long n = consume_literal_byte(s + pos, len - pos);
    if(n == 0) return 0;
    pos += n;
  }
  return pos;
}

/*
* Consume one varchar: ALPHA / DIGIT / "_" / pct-encoded per RFC 6570 section 2.3.
* Returns bytes consumed (1 or 3), or 0 on failure.
*/
static long parse_varchar(const char *s, long len) {
  if(len == 0) return 0;
  unsigned char c = (unsigned char)s[0];

  if(c == '%') return consume_pct_encoded(s, len);
  if(is_varchar_ascii(c)) return 1;
  return 0;
}

/*
* varname per RFC 6570 section 2.3:
*   varchar *( ["."] varchar )
* Dots are optional separators between varchars (no leading/trailing dot,
* no consecutive dots).
*/
static long parse_varname(const char *s, long len) {
  long first = parse_varchar(s, len);
  if(first == 0) return 0;

  long pos = first;

  while(pos < len) {
    if(s[pos] == '.') {
      long n = parse_varchar(s + pos + 1, len - pos - 1);
      if(n == 0) return 0;  // trailing dot or no varchar after dot
      pos += 1 + n;
      continue;
    }
    long n = parse_varchar(s + pos, len - pos);
    if(n == 0) break;
    pos += n;
  }

  return pos;
}

/*
* modifier-level4 per RFC 6570 section 2.4:
*   prefix = ":" max-length     ; max-length = %x31-39 0*3DIGIT (1..9999)
*   explode = "*"
* Caller must only invoke this when s[0] is ':' or '*'.
* Returns bytes consumed, or 0 on failure.
*/
static long parse_modifier(const char *s, long len) {
  if(len == 0) return 0;

  if(s[0] == '*') return 1;

  if(s[0] == ':') {
    if(len < 2) return 0;
    unsigned char first = (unsigned char)s[1];
    if(first < '1' || first > '9') return 0;

    long pos = 2;
    while(pos < len && IS_DIGIT((unsigned char)s[pos])) pos++;

    long digits = pos - 1;
    if(digits < 1 || digits > 4) return 0;
    return pos;
  }

  return 0;
}

/*
* varspec per RFC 6570 section 2.3:
*   varname [ modifier-level4 ]
*/
static long parse_varspec(const char *s, long len) {
  long n = parse_varname(s, len);
  if(n == 0) return 0;

  long pos = n;
  if(pos < len && (s[pos] == ':' || s[pos] == '*')) {
    long m = parse_modifier(s + pos, len - pos);
    if(m == 0) return 0;
    pos += m;
  }
  return pos;
}

/*
* variable-list per RFC 6570 section 2.3:
*   varspec *( "," varspec )
*/
static long parse_variable_list(const char *s, long len) {
  long n = parse_varspec(s, len);
  if(n == 0) return 0;

  long pos = n;
  while(pos < len && s[pos] == ',') {
    long m = parse_varspec(s + pos + 1, len - pos - 1);
    if(m == 0) return 0;
    pos += 1 + m;
  }
  return pos;
}

/*
* expression per RFC 6570 section 2.2:
*   "{" [ operator ] variable-list "}"
* Returns bytes consumed (including braces), or 0 on failure.
*/
static long parse_expression(const char *s, long len) {
  if(len < 2 || s[0] != '{') return 0;

  long pos = 1;

  if(pos < len && is_operator((unsigned char)s[pos])) pos++;

  long vl = parse_variable_list(s + pos, len - pos);
  if(vl == 0) return 0;
  pos += vl;

  if(pos >= len || s[pos] != '}') return 0;
  return pos + 1;
}

bool parse_uri_template(const char *s, long len) {
  long pos = 0;

  while(pos < len) {
    if(s[pos] == '{') {
      long n = parse_expression(s + pos, len - pos);
      if(n == 0) return false;
      pos += n;
    } else {
      long n = parse_literals_run(s + pos, len - pos);
      if(n == 0) return false;
      pos += n;
    }
  }

  return true;
}
