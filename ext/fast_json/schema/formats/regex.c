#include "regex.h"
#include "error.h"

#include <ruby.h>

/*
* JSON Schema section 7.3.8 cites ECMA-262 section 21.2 as the regex dialect.
*
* This implementation delegates to Ruby's Regexp.new (Onigmo engine), which
* differs from strict ECMA-262 in some edge cases. 
*
* The divergence matches the behavior of other Ruby JSON Schema validators.
*/
static VALUE try_compile_regex(VALUE data) {
  return rb_reg_new_str(data, 0);
}

void validate_format_regex(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  int state = 0;
  rb_protect(try_compile_regex, data, &state);

  if(state) {
    rb_set_errinfo(Qnil);

    yield_error(compiled_schema, data, context, "format_regex");
  }
}
