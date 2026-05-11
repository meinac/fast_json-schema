#include "formats/utils/hostname_parser.h"
#include "formats/utils/utf8.h"

#include <stdbool.h>

#define MAX_HOSTNAME_LEN 253
#define MAX_LABEL_LEN     63

#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9')
#define IS_ALPHA(c) (((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))

/*
* let-dig per RFC 1123 section 2.1: ALPHA / DIGIT.
*/
static bool is_let_dig(unsigned char c) {
  return IS_ALPHA(c) || IS_DIGIT(c);
}

/*
* let-dig-hyp per RFC 1123 section 2.1: ALPHA / DIGIT / "-".
*/
static bool is_let_dig_hyp(unsigned char c) {
  return is_let_dig(c) || c == '-';
}

/*
* Try to consume one "label character" starting at s.
* Returns the number of bytes consumed, or 0 if the input does not start
* with a valid label character.
*
* In allow_utf8 mode, any byte >= 0x80 must begin a valid UTF-8 sequence
* and is accepted as a single label character (pragmatic idn-hostname).
*/
static long consume_label_char(const char *s, long len, bool allow_utf8, bool allow_hyphen) {
  unsigned char c = (unsigned char)s[0];

  if(c < 0x80) {
    if(allow_hyphen) return is_let_dig_hyp(c) ? 1 : 0;

    return is_let_dig(c) ? 1 : 0;
  }

  if(!allow_utf8) return 0;

  return utf8_seq_len((const unsigned char *)s, len);
}

/*
* Parse a single label: let-dig [ *(let-dig-hyp) let-dig ].
* The label may not start with a hyphen and may not end with a hyphen.
* Returns the number of bytes consumed, or 0 on failure.
*/
static long parse_label(const char *s, long len, bool allow_utf8) {
  if(len == 0) return 0;

  long first = consume_label_char(s, len, allow_utf8, false);

  if(first == 0) return 0;

  long pos = first;
  long last_char_start = 0;

  while(pos < len && s[pos] != '.') {
    long n = consume_label_char(s + pos, len - pos, allow_utf8, true);

    if(n == 0) return 0;

    last_char_start = pos;
    pos += n;

    if(pos > MAX_LABEL_LEN) return 0;
  }

  /*
  * Label must not end with a hyphen. The last consumed character begins at
  * either `last_char_start` (if more than one character) or at offset 0.
  */
  long last_start = last_char_start ? last_char_start : 0;

  if(pos > first && (unsigned char)s[last_start] == '-') return 0;

  return pos;
}

bool parse_hostname(const char *s, long len, bool allow_utf8) {
  if(len == 0) return false;
  if(len > MAX_HOSTNAME_LEN) return false;

  long pos = 0;

  while(pos < len) {
    long label_len = parse_label(s + pos, len - pos, allow_utf8);

    if(label_len == 0) return false;

    pos += label_len;

    if(pos == len) break;

    if(s[pos] != '.') return false;

    pos++; // consume '.'

    /*
    * A single trailing dot is accepted (RFC 1034 section 3.1 absolute form).
    * Any further bytes after the trailing dot, or two consecutive dots,
    * will be rejected because parse_label returns 0 on empty input.
    */
    if(pos == len) return true;
  }

  return true;
}
