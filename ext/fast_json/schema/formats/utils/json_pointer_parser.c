#include "formats/utils/json_pointer_parser.h"
#include "formats/utils/utf8.h"

bool parse_json_pointer(const char *s, long len) {
  long pos = 0;

  while(pos < len) {
    if(s[pos] != '/') return false;

    pos++;

    while(pos < len && s[pos] != '/') {
      unsigned char c = (unsigned char)s[pos];

      /* 
      * Escape sequence: "~" ( "0" / "1" )
      */
      if(c == '~') {
        if(pos + 1 >= len) return false;
        if(s[pos + 1] != '0' && s[pos + 1] != '1') return false;
        pos += 2;
        continue;
      }

      /*
      * ASCII byte
      */
      if(c < 0x80) {
        pos++;
        continue;
      }

      /*
      * UTF-8 multi-byte sequence
      */
      long n = utf8_seq_len((const unsigned char *)(s + pos), len - pos);

      if(n == 0) return false;

      pos += n;
    }
  }

  return true;
}
