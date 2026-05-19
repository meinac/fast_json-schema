#include "validate.h"
#include "error.h"

void no_op_validate(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  return;
}

void false_validate(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  yield_error(compiled_schema, data, context, "false_schema");
}

void validate_by_data_type(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(RB_INTEGER_TYPE_P(data)) {
    validate_integer(schema, compiled_schema, data, context);
  } else if(RB_TYPE_P(data, T_FLOAT)) {
    validate_number(schema, compiled_schema, data, context);
  } else if(RB_TYPE_P(data, T_STRING)) {
    validate_string(schema, compiled_schema, data, context);
  } else if(RB_TYPE_P(data, T_HASH)) {
    validate_object(schema, compiled_schema, data, context);
  } else if(RB_TYPE_P(data, T_ARRAY)) {
    validate_array(schema, compiled_schema, data, context);
  }
}

void validate_by_type_list(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  unsigned int flags = compiled_schema->type_flags;

  if(NIL_P(data)) {
    if(flags & TYPE_NULL) {
      validate_null(schema, compiled_schema, data, context);
    } else {
      yield_error(compiled_schema, data, context, "type");
    }
  } else if(data == Qtrue || data == Qfalse) {
    if(flags & TYPE_BOOL) {
      validate_bool(schema, compiled_schema, data, context);
    } else {
      yield_error(compiled_schema, data, context, "type");
    }
  } else if(RB_INTEGER_TYPE_P(data)) {
    if(flags & TYPE_INTEGER) {
      validate_integer(schema, compiled_schema, data, context);
    } else if(flags & TYPE_NUMBER) {
      validate_number(schema, compiled_schema, data, context);
    } else {
      yield_error(compiled_schema, data, context, "type");
    }
  } else if(RB_TYPE_P(data, T_FLOAT)) {
    if(flags & TYPE_NUMBER) {
      validate_number(schema, compiled_schema, data, context);
    } else {
      yield_error(compiled_schema, data, context, "type");
    }
  } else if(RB_TYPE_P(data, T_STRING)) {
    if(flags & TYPE_STRING) {
      validate_string(schema, compiled_schema, data, context);
    } else {
      yield_error(compiled_schema, data, context, "type");
    }
  } else if(RB_TYPE_P(data, T_HASH)) {
    if(flags & TYPE_OBJECT) {
      validate_object(schema, compiled_schema, data, context);
    } else {
      yield_error(compiled_schema, data, context, "type");
    }
  } else if(RB_TYPE_P(data, T_ARRAY)) {
    if(flags & TYPE_ARRAY) {
      validate_array(schema, compiled_schema, data, context);
    } else {
      yield_error(compiled_schema, data, context, "type");
    }
  }
}

static void validate_const(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  VALUE equal = rb_funcall(compiled_schema->const_val, rb_intern("=="), 1, data);

  if(equal == Qfalse)
    yield_error(compiled_schema, data, context, "const");
}

static void validate_enum(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  VALUE include = rb_funcall(compiled_schema->enum_val, rb_intern("include?"), 1, data);

  if(include == Qfalse)
    yield_error(compiled_schema, data, context, "enum");
}

void validate(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  compiled_schema->type_validation_function(schema, compiled_schema, data, context);

  if(compiled_schema->const_val != Qundef)
    validate_const(schema, compiled_schema, data, context);

  if(compiled_schema->enum_val != Qundef)
    validate_enum(schema, compiled_schema, data, context);

  if(compiled_schema->if_schema != NULL)
    validate_if(schema, compiled_schema, data, context);

  if(compiled_schema->not_schema != NULL)
    validate_not(schema, compiled_schema, data, context);

  if(compiled_schema->allOf_val != Qundef)
    validate_all_of(schema, compiled_schema, data, context);

  if(compiled_schema->anyOf_val != Qundef)
    validate_any_of(schema, compiled_schema, data, context);

  if(compiled_schema->oneOf_val != Qundef)
    validate_one_of(schema, compiled_schema, data, context);
}
