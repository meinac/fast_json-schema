#include "ref_decoder.h"

static VALUE parser;

/*
 * Decode a $ref so it matches the keys we store in `ref_data`.
 *
 * A $ref is a URI Reference (RFC 3986). Its fragment is a JSON Pointer
 * (RFC 6901). Both layers can encode characters:
 *
 *   - JSON Pointer escapes '/' as '~1' and '~' as '~0' so we need to replace:
 *     - '~1' with '/'
 *     - '~0' with '~'
 *   - URI fragments may percent-encode any character.
 *
 * We register paths in `ref_data` using their literal characters, so we
 * must reverse both layers before the lookup.
 *
 * Order matters per RFC 6901 section 4: replace '~1' first, then '~0'.
 * Doing it the other way would turn '~01' into '/', not '~1'.
 */
VALUE decode_ref(VALUE ref) {
  StringValue(ref);

  VALUE decoded = rb_funcall(parser, rb_intern("unescape"), 1, ref);

  decoded = rb_funcall(decoded, rb_intern("gsub"), 2, rb_str_new_cstr("~1"), rb_str_new_cstr("/"));
  decoded = rb_funcall(decoded, rb_intern("gsub"), 2, rb_str_new_cstr("~0"), rb_str_new_cstr("~"));

  return decoded;
}

void Init_ref_decoder(void) {
  rb_require("uri");

  VALUE uri_module = rb_const_get(rb_cObject, rb_intern("URI"));

  parser = rb_const_get(uri_module, rb_intern("DEFAULT_PARSER"));

  rb_gc_register_address(&parser);
}
