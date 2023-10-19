#include "validate.h"
#include "error.h"

bool is_valid(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  int result;
  ValidationEnv old_env;
  SET_JUMP(context->env, old_env, result);

  if (result) {
    RESET_JUMP(context->env, old_env);

    return false;
  } else {
    compiled_schema->validation_function(schema, compiled_schema, data, context);
  }

  RESET_JUMP(context->env, old_env);

  return true;
}

void no_op_validate(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  return;
}

void false_validate(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  yield_error(compiled_schema, data, context, "false_schema");
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
