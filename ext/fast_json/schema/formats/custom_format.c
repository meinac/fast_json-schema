#include "custom_format.h"
#include "error.h"

struct custom_call_args {
  VALUE callable;
  VALUE data;
};

static VALUE call_custom_validator(VALUE arg_ptr) {
  struct custom_call_args *args = (struct custom_call_args *)arg_ptr;

  return rb_funcall(args->callable, rb_intern("call"), 1, args->data);
}

static void validate_custom_format(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {  
  int state = 0;
  struct custom_call_args args = { compiled_schema->custom_format_callable_val, data };
  VALUE result = rb_protect(call_custom_validator, (VALUE)&args, &state);

  const char *error_key = RSTRING_PTR(compiled_schema->custom_format_error_key_val);

  if(state) {
    rb_set_errinfo(Qnil);

    yield_error(compiled_schema, data, context, error_key);
    return;
  }

  if(!RTEST(result))
    yield_error(compiled_schema, data, context, error_key);
}

static VALUE custom_format_validation_callable_for(VALUE format_val, VALUE custom_formats) {
  if(NIL_P(custom_formats) || custom_formats == Qundef)
    return Qundef;

  VALUE callable = rb_hash_aref(custom_formats, format_val);

  if(NIL_P(callable))
    return Qundef;

  return callable;
}

static VALUE custom_format_error_key_for(VALUE format_val) {
  VALUE error_key = rb_str_new_cstr("format_");
  rb_str_append(error_key, format_val);
  rb_str_freeze(error_key);

  return error_key;
}

bool set_custom_format_validation_function_for_compiled_schema(CompiledSchema *compiled_schema, VALUE format_val, VALUE custom_formats) {
  VALUE callable = custom_format_validation_callable_for(format_val, custom_formats);

  if(Qundef == callable) return false;

  compiled_schema->format_validation_function = validate_custom_format;
  compiled_schema->custom_format_callable_val = callable;
  compiled_schema->custom_format_error_key_val = custom_format_error_key_for(format_val);

  return true;
}
