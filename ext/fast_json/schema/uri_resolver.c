#include "uri_resolver.h"

static VALUE uri_module;
static ID id_join;
static ID id_to_s;

bool uri_resolver_is_absolute(VALUE ref_str) {
  if (!RB_TYPE_P(ref_str, T_STRING)) return false;

  long len = RSTRING_LEN(ref_str);
  const char *p = RSTRING_PTR(ref_str);

  if (len < 2) return false;
  if (!((p[0] >= 'a' && p[0] <= 'z') || (p[0] >= 'A' && p[0] <= 'Z'))) return false;

  for (long i = 1; i < len; i++) {
    char ch = p[i];

    if (ch == ':') return i > 0;
    if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
          (ch >= '0' && ch <= '9') || ch == '+' || ch == '-' || ch == '.')) {
      return false;
    }
  }

  return false;
}

struct join_args {
  VALUE base;
  VALUE ref;
};

static VALUE do_uri_join(VALUE data) {
  struct join_args *args = (struct join_args *)data;

  // Join the base and reference URIs.
  VALUE joined = rb_funcall(uri_module, id_join, 2, args->base, args->ref);

  // Convert the joined URI to a string.
  return rb_funcall(joined, id_to_s, 0);
}

VALUE uri_resolver_join(VALUE base, VALUE ref) {
  if (NIL_P(base) || base == Qundef) return Qnil;
  if (NIL_P(ref) || ref == Qundef) return Qnil;
  if (!RB_TYPE_P(base, T_STRING) || !RB_TYPE_P(ref, T_STRING)) return Qnil;
  if (RSTRING_LEN(base) == 0) return Qnil;

  struct join_args args = { base, ref };
  int state = 0;

  // Protect against exceptions raised by Ruby's URI.join.
  VALUE result = rb_protect(do_uri_join, (VALUE)&args, &state);

  if (state) {
    // Clear the error information to avoid propagating the exception.
    rb_set_errinfo(Qnil);
    return Qnil;
  }

  return result;
}

void Init_uri_resolver(void) {
  rb_require("uri");

  uri_module = rb_const_get(rb_cObject, rb_intern("URI"));
  rb_gc_register_address(&uri_module);

  id_join = rb_intern("join");
  id_to_s = rb_intern("to_s");
}
