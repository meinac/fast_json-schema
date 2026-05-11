#include "formats/utils/utf8.h"

/* Continuation-byte range (RFC 3629). */
#define UTF8_CONT_MIN         0x80   /* 1000 0000 */
#define UTF8_CONT_MAX         0xBF   /* 1011 1111 */

/* ASCII upper bound. */
#define UTF8_ASCII_MAX        0x7F   /* 0111 1111 */

/* Valid lead-byte ranges per sequence length. */
#define UTF8_LEAD2_MIN        0xC2   /* 1100 0010 */
#define UTF8_LEAD2_MAX        0xDF   /* 1101 1111 */
#define UTF8_LEAD3_MIN        0xE0   /* 1110 0000 */
#define UTF8_LEAD3_MAX        0xEF   /* 1110 1111 */
#define UTF8_LEAD4_MIN        0xF0   /* 1111 0000 */
#define UTF8_LEAD4_MAX        0xF4   /* 1111 0100 */

/* Lead byte triggering byte-2 anti-surrogate tightening (U+D800..U+DFFF). */
#define UTF8_LEAD3_SURROGATE  0xED   /* 1110 1101 */

/* Tightened byte-2 bounds for irregular lead bytes. */
#define UTF8_E0_BYTE2_MIN     0xA0   /* 1010 0000 */
#define UTF8_ED_BYTE2_MAX     0x9F   /* 1001 1111 */
#define UTF8_F0_BYTE2_MIN     0x90   /* 1001 0000 */
#define UTF8_F4_BYTE2_MAX     0x8F   /* 1000 1111 */

long utf8_seq_len(const unsigned char *s, long remaining) {
  if(remaining < 1) return 0;

  unsigned char first_byte = s[0];

  if(first_byte <= UTF8_ASCII_MAX) return 1;

  if(first_byte < UTF8_LEAD2_MIN) return 0;

  if(first_byte <= UTF8_LEAD2_MAX) {
    if(remaining < 2) return 0;
    if(s[1] < UTF8_CONT_MIN || s[1] > UTF8_CONT_MAX) return 0;

    return 2;
  }

  if(first_byte <= UTF8_LEAD3_MAX) {
    if(remaining < 3) return 0;

    unsigned char low = UTF8_CONT_MIN, high = UTF8_CONT_MAX;

    if(first_byte == UTF8_LEAD3_MIN) low = UTF8_E0_BYTE2_MIN;        /* anti-overlong */
    if(first_byte == UTF8_LEAD3_SURROGATE) high = UTF8_ED_BYTE2_MAX; /* anti-surrogate */

    if(s[1] < low || s[1] > high) return 0;
    if(s[2] < UTF8_CONT_MIN || s[2] > UTF8_CONT_MAX) return 0;

    return 3;
  }

  if(first_byte <= UTF8_LEAD4_MAX) {
    if(remaining < 4) return 0;

    unsigned char low = UTF8_CONT_MIN, high = UTF8_CONT_MAX;

    if(first_byte == UTF8_LEAD4_MIN) low = UTF8_F0_BYTE2_MIN;  /* anti-overlong */
    if(first_byte == UTF8_LEAD4_MAX) high = UTF8_F4_BYTE2_MAX; /* anti-codepoint > U+10FFFF */

    if(s[1] < low || s[1] > high) return 0;
    if(s[2] < UTF8_CONT_MIN || s[2] > UTF8_CONT_MAX) return 0;
    if(s[3] < UTF8_CONT_MIN || s[3] > UTF8_CONT_MAX) return 0;

    return 4;
  }

  return 0;
}
