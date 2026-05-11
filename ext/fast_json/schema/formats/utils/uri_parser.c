#include "formats/utils/uri_parser.h"
#include "formats/utils/ip_parser.h"
#include "formats/utils/utf8.h"

#include <stdbool.h>

#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9')
#define IS_ALPHA(c) (((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))
#define IS_HEX(c)   (IS_DIGIT(c) || ((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f'))

/*
* unreserved per RFC 3986 section 2.3:
*   ALPHA / DIGIT / "-" / "." / "_" / "~"
*/
static bool is_unreserved(unsigned char c) {
  if(IS_ALPHA(c) || IS_DIGIT(c)) return true;

  switch(c) {
    case '-': case '.': case '_': case '~':
      return true;
  }

  return false;
}

/*
* sub-delims per RFC 3986 section 2.2:
*   "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
*/
static bool is_sub_delim(unsigned char c) {
  switch(c) {
    case '!': case '$': case '&': case '\'': case '(': case ')':
    case '*': case '+': case ',': case ';': case '=':
      return true;
  }

  return false;
}

/*
* scheme character per RFC 3986 section 3.1:
*   ALPHA / DIGIT / "+" / "-" / "."
*/
static bool is_scheme_char(unsigned char c) {
  return IS_ALPHA(c) || IS_DIGIT(c) || c == '+' || c == '-' || c == '.';
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
* Consume a UTF-8 multi-byte sequence when allow_utf8 is true.
* Returns the byte length on success, or 0 if not a valid UTF-8 sequence
* (or if allow_utf8 is false).
*/
static long consume_utf8(const char *s, long len, bool allow_utf8) {
  if(!allow_utf8) return 0;

  unsigned char c = (unsigned char)s[0];
  if(c < 0x80) return 0;

  long n = utf8_seq_len((const unsigned char *)s, len);
  return n >= 2 ? n : 0;
}

/*
* pchar per RFC 3986 section 3.3:
*   unreserved / pct-encoded / sub-delims / ":" / "@"
* Returns bytes consumed, or 0 on failure.
*/
static long consume_pchar(const char *s, long len, bool allow_utf8) {
  unsigned char c = (unsigned char)s[0];

  if(c < 0x80) {
    if(is_unreserved(c)) return 1;
    if(is_sub_delim(c))  return 1;
    if(c == ':' || c == '@') return 1;
    if(c == '%') return consume_pct_encoded(s, len);

    return 0;
  }

  return consume_utf8(s, len, allow_utf8);
}

/*
* segment-nz-nc per RFC 3986 section 3.3 (relative-part):
*   1*( unreserved / pct-encoded / sub-delims / "@" )
* — i.e., pchar without ":".
*/
static long consume_pchar_nc(const char *s, long len, bool allow_utf8) {
  unsigned char c = (unsigned char)s[0];

  if(c == ':') return 0;

  return consume_pchar(s, len, allow_utf8);
}

/*
* scheme per RFC 3986 section 3.1:
*   ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
* Returns bytes consumed, or 0 on failure.
*/
static long parse_scheme(const char *s, long len) {
  if(len == 0) return 0;
  if(!IS_ALPHA((unsigned char)s[0])) return 0;

  long pos = 1;
  while(pos < len && is_scheme_char((unsigned char)s[pos])) pos++;

  return pos;
}

/*
* IPvFuture per RFC 3986 section 3.2.2:
*   "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
* Returns bytes consumed, or 0 on failure.
*/
static long parse_ipv_future(const char *s, long len) {
  if(len < 4) return 0;
  if(s[0] != 'v' && s[0] != 'V') return 0;

  long pos = 1;
  long hex_start = pos;

  while(pos < len && IS_HEX((unsigned char)s[pos])) pos++;
  if(pos == hex_start) return 0;
  if(pos >= len || s[pos] != '.') return 0;

  pos++;
  long tail_start = pos;

  while(pos < len) {
    unsigned char c = (unsigned char)s[pos];
    if(is_unreserved(c) || is_sub_delim(c) || c == ':') {
      pos++;
      continue;
    }
    break;
  }

  if(pos == tail_start) return 0;

  return pos;
}

/*
* IP-literal per RFC 3986 section 3.2.2:
*   "[" ( IPv6address / IPvFuture ) "]"
* Returns bytes consumed (including brackets), or 0 on failure.
*/
static long parse_ip_literal(const char *s, long len) {
  if(len < 2 || s[0] != '[') return 0;

  long end = -1;
  for(long i = 1; i < len; i++) {
    if(s[i] == ']') { end = i; break; }
  }
  if(end == -1) return 0;

  long body_len = end - 1;
  const char *body = s + 1;

  if(body_len >= 1 && (body[0] == 'v' || body[0] == 'V')) {
    if(parse_ipv_future(body, body_len) == body_len) return end + 1;
    return 0;
  }

  if(parse_ipv6(body, body_len)) return end + 1;
  return 0;
}

/*
* reg-name per RFC 3986 section 3.2.2:
*   *( unreserved / pct-encoded / sub-delims )
* Returns bytes consumed (may be 0 — empty reg-name is valid).
*/
static long parse_reg_name(const char *s, long len, bool allow_utf8) {
  long pos = 0;

  while(pos < len) {
    unsigned char c = (unsigned char)s[pos];

    if(c < 0x80) {
      if(is_unreserved(c) || is_sub_delim(c)) {
        pos++;
        continue;
      }
      if(c == '%') {
        long n = consume_pct_encoded(s + pos, len - pos);
        if(n == 0) break;
        pos += n;
        continue;
      }
      break;
    }

    long n = consume_utf8(s + pos, len - pos, allow_utf8);
    if(n == 0) break;
    pos += n;
  }

  return pos;
}

/*
* host per RFC 3986 section 3.2.2: IP-literal / IPv4address / reg-name.
* Returns bytes consumed (may be 0 — empty host is valid).
*/
static long parse_host(const char *s, long len, bool allow_utf8) {
  if(len == 0) return 0;

  if(s[0] == '[') {
    long n = parse_ip_literal(s, len);
    return n;
  }

  /*
  * Try IPv4 first if it can match the longest prefix that looks like one
  * (i.e., the run of digits and dots). If it matches a prefix exactly equal
  * to that run, accept; otherwise fall through to reg-name (which is
  * more permissive and would also accept dotted-decimal byte sequences).
  */
  long digit_dot_run = 0;
  while(digit_dot_run < len) {
    unsigned char c = (unsigned char)s[digit_dot_run];
    if(IS_DIGIT(c) || c == '.') {
      digit_dot_run++;
      continue;
    }
    break;
  }

  if(digit_dot_run > 0 && parse_ipv4(s, digit_dot_run)) {
    return digit_dot_run;
  }

  return parse_reg_name(s, len, allow_utf8);
}

/*
* port per RFC 3986 section 3.2.3: *DIGIT (may be empty).
*/
static long parse_port(const char *s, long len) {
  long pos = 0;
  while(pos < len && IS_DIGIT((unsigned char)s[pos])) pos++;
  return pos;
}

/*
* userinfo per RFC 3986 section 3.2.1:
*   *( unreserved / pct-encoded / sub-delims / ":" )
*/
static long parse_userinfo(const char *s, long len, bool allow_utf8) {
  long pos = 0;

  while(pos < len) {
    unsigned char c = (unsigned char)s[pos];

    if(c < 0x80) {
      if(is_unreserved(c) || is_sub_delim(c) || c == ':') {
        pos++;
        continue;
      }
      if(c == '%') {
        long n = consume_pct_encoded(s + pos, len - pos);
        if(n == 0) break;
        pos += n;
        continue;
      }
      break;
    }

    long n = consume_utf8(s + pos, len - pos, allow_utf8);
    if(n == 0) break;
    pos += n;
  }

  return pos;
}

/*
* authority per RFC 3986 section 3.2:
*   [ userinfo "@" ] host [ ":" port ]
* Returns bytes consumed (must consume up to the next "/", "?", "#", or end).
*/
static long parse_authority(const char *s, long len, bool allow_utf8) {
  long pos = 0;

  /* Find the end of authority: next "/", "?", "#", or end. */
  long auth_end = len;
  for(long i = 0; i < len; i++) {
    if(s[i] == '/' || s[i] == '?' || s[i] == '#') { auth_end = i; break; }
  }

  /* Optional userinfo terminated by "@". Search within auth_end only. */
  long at_pos = -1;
  for(long i = 0; i < auth_end; i++) {
    if(s[i] == '@') { at_pos = i; break; }
  }

  if(at_pos >= 0) {
    if(parse_userinfo(s, at_pos, allow_utf8) != at_pos) return 0;
    pos = at_pos + 1;
  }

  /* host: either IP-literal "[...]", or a sequence ending at ":" or auth_end. */
  long host_start = pos;
  long host_end;

  if(pos < auth_end && s[pos] == '[') {
    long lit = parse_ip_literal(s + pos, auth_end - pos);
    if(lit == 0) return 0;
    host_end = pos + lit;
  } else {
    /* host runs up to ":" (port separator) or auth_end. */
    host_end = pos;
    while(host_end < auth_end && s[host_end] != ':') host_end++;

    long host_len = parse_host(s + pos, host_end - pos, allow_utf8);
    if(host_len != host_end - pos) return 0;
  }

  pos = host_end;

  /* Optional ":" port */
  if(pos < auth_end && s[pos] == ':') {
    pos++;
    long port_len = parse_port(s + pos, auth_end - pos);
    if(pos + port_len != auth_end) return 0;
    pos += port_len;
  }

  if(pos != auth_end) return 0;

  (void)host_start;
  return pos;
}

/*
* segment per RFC 3986 section 3.3: *pchar
*/
static long parse_segment(const char *s, long len, bool allow_utf8) {
  long pos = 0;
  while(pos < len) {
    long n = consume_pchar(s + pos, len - pos, allow_utf8);
    if(n == 0) break;
    pos += n;
  }
  return pos;
}

/*
* segment-nz per RFC 3986 section 3.3: 1*pchar
*/
static long parse_segment_nz(const char *s, long len, bool allow_utf8) {
  long pos = parse_segment(s, len, allow_utf8);
  return pos > 0 ? pos : 0;
}

/*
* segment-nz-nc per RFC 3986 section 3.3:
*   1*( unreserved / pct-encoded / sub-delims / "@" )  (no colon)
*/
static long parse_segment_nz_nc(const char *s, long len, bool allow_utf8) {
  long pos = 0;
  while(pos < len) {
    long n = consume_pchar_nc(s + pos, len - pos, allow_utf8);
    if(n == 0) break;
    pos += n;
  }
  return pos > 0 ? pos : 0;
}

/*
* path-abempty per RFC 3986 section 3.3:
*   *( "/" segment )
* Always succeeds (may consume zero bytes).
*/
static long parse_path_abempty(const char *s, long len, bool allow_utf8) {
  long pos = 0;
  while(pos < len && s[pos] == '/') {
    pos++;
    pos += parse_segment(s + pos, len - pos, allow_utf8);
  }
  return pos;
}

/*
* path-absolute per RFC 3986 section 3.3:
*   "/" [ segment-nz *( "/" segment ) ]
*/
static long parse_path_absolute(const char *s, long len, bool allow_utf8) {
  if(len < 1 || s[0] != '/') return 0;
  long pos = 1;

  long nz = parse_segment_nz(s + pos, len - pos, allow_utf8);
  if(nz == 0) return pos;  /* just "/" is valid */

  pos += nz;
  while(pos < len && s[pos] == '/') {
    pos++;
    pos += parse_segment(s + pos, len - pos, allow_utf8);
  }
  return pos;
}

/*
* path-rootless per RFC 3986 section 3.3:
*   segment-nz *( "/" segment )
*/
static long parse_path_rootless(const char *s, long len, bool allow_utf8) {
  long nz = parse_segment_nz(s, len, allow_utf8);
  if(nz == 0) return 0;

  long pos = nz;
  while(pos < len && s[pos] == '/') {
    pos++;
    pos += parse_segment(s + pos, len - pos, allow_utf8);
  }
  return pos;
}

/*
* path-noscheme per RFC 3986 section 4.2:
*   segment-nz-nc *( "/" segment )
*/
static long parse_path_noscheme(const char *s, long len, bool allow_utf8) {
  long nz = parse_segment_nz_nc(s, len, allow_utf8);
  if(nz == 0) return 0;

  long pos = nz;
  while(pos < len && s[pos] == '/') {
    pos++;
    pos += parse_segment(s + pos, len - pos, allow_utf8);
  }
  return pos;
}

/*
* query / fragment per RFC 3986 section 3.4, section 3.5:
*   *( pchar / "/" / "?" )
*/
static long parse_query_or_fragment(const char *s, long len, bool allow_utf8) {
  long pos = 0;
  while(pos < len) {
    unsigned char c = (unsigned char)s[pos];
    if(c == '/' || c == '?') {
      pos++;
      continue;
    }
    long n = consume_pchar(s + pos, len - pos, allow_utf8);
    if(n == 0) break;
    pos += n;
  }
  return pos;
}

/*
* Parse hier-part or relative-part starting at s, consuming through the
* end of the path component (i.e., up to "?", "#", or end of input).
*
* Selected variant:
*   - If starts with "//": authority + path-abempty.
*   - If starts with "/":  path-absolute.
*   - Else if is_relative: path-noscheme.
*   - Else:                path-rootless or empty.
*
* Returns the number of bytes consumed, or -1 on failure.
*/
static long parse_hier_or_relative_part(const char *s, long len, bool allow_utf8, bool is_relative) {
  if(len == 0) return 0;  /* path-empty */

  /* Boundary of the path component: up to "?" or "#". */
  long path_end = len;
  for(long i = 0; i < len; i++) {
    if(s[i] == '?' || s[i] == '#') { path_end = i; break; }
  }

  long pos = 0;

  if(path_end >= 2 && s[0] == '/' && s[1] == '/') {
    pos = 2;
    long auth_end = path_end;
    /* Find end of authority (next "/" within path_end). */
    for(long i = pos; i < path_end; i++) {
      if(s[i] == '/') { auth_end = i; break; }
    }
    long auth_len = parse_authority(s + pos, auth_end - pos, allow_utf8);
    if(auth_len != auth_end - pos) return -1;
    pos = auth_end;
    pos += parse_path_abempty(s + pos, path_end - pos, allow_utf8);
    if(pos != path_end) return -1;
    return pos;
  }

  if(path_end >= 1 && s[0] == '/') {
    long n = parse_path_absolute(s, path_end, allow_utf8);
    if(n != path_end) return -1;
    return n;
  }

  if(path_end == 0) return 0;  /* path-empty */

  if(is_relative) {
    long n = parse_path_noscheme(s, path_end, allow_utf8);
    if(n != path_end) return -1;
    return n;
  }

  long n = parse_path_rootless(s, path_end, allow_utf8);
  if(n != path_end) return -1;
  return n;
}

/*
* Try to parse a full URI starting at s: scheme ":" hier-part [ "?" query ] [ "#" fragment ].
* Returns true if the entire input is consumed, false otherwise.
*/
static bool try_parse_full_uri(const char *s, long len, bool allow_utf8) {
  long scheme_len = parse_scheme(s, len);
  if(scheme_len == 0) return false;
  if(scheme_len >= len || s[scheme_len] != ':') return false;

  long pos = scheme_len + 1;

  long hp = parse_hier_or_relative_part(s + pos, len - pos, allow_utf8, false);
  if(hp < 0) return false;

  /* But parse_hier_or_relative_part stops at the first "?" or "#" — we need
  * to walk to the end of the current path component. parse_hier_or_relative_part
  * already enforces that it consumes everything up to "?", "#", or end of its
  * window, so hp equals the length of the hier-part subset before "?" / "#". */
  pos += hp;

  if(pos < len && s[pos] == '?') {
    pos++;
    /* query runs to "#" or end. */
    long q_end = len;
    for(long i = pos; i < len; i++) {
      if(s[i] == '#') { q_end = i; break; }
    }
    long q_len = parse_query_or_fragment(s + pos, q_end - pos, allow_utf8);
    if(pos + q_len != q_end) return false;
    pos = q_end;
  }

  if(pos < len && s[pos] == '#') {
    pos++;
    long f_len = parse_query_or_fragment(s + pos, len - pos, allow_utf8);
    if(pos + f_len != len) return false;
    pos = len;
  }

  return pos == len;
}

/*
* Try to parse a relative-ref: relative-part [ "?" query ] [ "#" fragment ].
* Returns true if the entire input is consumed, false otherwise.
*/
static bool try_parse_relative_ref(const char *s, long len, bool allow_utf8) {
  long rp = parse_hier_or_relative_part(s, len, allow_utf8, true);
  if(rp < 0) return false;

  long pos = rp;

  if(pos < len && s[pos] == '?') {
    pos++;
    long q_end = len;
    for(long i = pos; i < len; i++) {
      if(s[i] == '#') { q_end = i; break; }
    }
    long q_len = parse_query_or_fragment(s + pos, q_end - pos, allow_utf8);
    if(pos + q_len != q_end) return false;
    pos = q_end;
  }

  if(pos < len && s[pos] == '#') {
    pos++;
    long f_len = parse_query_or_fragment(s + pos, len - pos, allow_utf8);
    if(pos + f_len != len) return false;
    pos = len;
  }

  return pos == len;
}

bool parse_uri_reference(const char *s, long len, bool require_scheme, bool allow_utf8) {
  if(len == 0) return !require_scheme;  /* empty is valid only for URI-reference */

  if(try_parse_full_uri(s, len, allow_utf8)) return true;

  if(require_scheme) return false;

  return try_parse_relative_ref(s, len, allow_utf8);
}
