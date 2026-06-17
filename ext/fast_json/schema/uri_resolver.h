#include <ruby.h>
#include <stdbool.h>

void Init_uri_resolver(void);

// Returns true if `ref_str` starts with a URI scheme (e.g. "http:", "urn:").
bool uri_resolver_is_absolute(VALUE ref_str);

/*
* Joins `ref` against `base` using Ruby's URI.join. Returns the resolved
* absolute URI string, or Qnil if `base` is Qnil/empty, `ref` is Qnil/empty,
* or the underlying call raised an exception.
*/
VALUE uri_resolver_join(VALUE base, VALUE ref);
