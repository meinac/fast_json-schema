#ifndef FAST_JSON_FORMATS_UTILS_UTF8_H
#define FAST_JSON_FORMATS_UTILS_UTF8_H

#include <stdbool.h>

/*
* Strict UTF-8 sequence validator. Given a pointer to the leading byte of a
* UTF-8 character, returns the byte length (1..4) of a valid UTF-8 sequence
* starting at that position, or 0 if the bytes do not form a valid sequence.
*
* Returns 1 for any ASCII byte (< 0x80). Returns 2..4 for valid multi-byte
* sequences. Rejects overlong encodings, UTF-16 surrogates (U+D800..U+DFFF)
* and codepoints above U+10FFFF.
*/
long utf8_seq_len(const unsigned char *s, long remaining);

#endif
