#include "validate_object.h"
#include "error.h"

struct memo_S {
  VALUE schema;
  CompiledSchema *compiled_schema;
  Context *context;
};

static int validate_object_property(VALUE key, VALUE value, VALUE data) {
  if(!RB_TYPE_P(key, T_STRING)) return ST_CONTINUE;

  struct memo_S *memo = (struct memo_S*)data;
  VALUE properties_val = memo->compiled_schema->properties_val;
  VALUE child_compiled_schema_obj = rb_hash_aref(properties_val, key);

  if(NIL_P(child_compiled_schema_obj)) return ST_CONTINUE;

  CompiledSchema *child_compiled_schema;
  GetCompiledSchema(child_compiled_schema_obj, child_compiled_schema);

  ADD_TO_CONTEXT(memo->context, key);

  child_compiled_schema->validation_function(memo->schema, child_compiled_schema, value, memo->context);

  return ST_CONTINUE;
}

static void validate_properties(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  struct memo_S memo = { schema, compiled_schema, context };

  INCR_CONTEXT(context);

  rb_hash_foreach(data, validate_object_property, (VALUE)&memo);

  DECR_CONTEXT(context);
}

static void validate_required(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  long i;
  VALUE required_val = compiled_schema->required_val;

  for(i = 0; i < RARRAY_LEN(required_val); i++) {
    VALUE entry = rb_ary_entry(required_val, i);

    if(rb_hash_lookup2(data, entry, Qundef) == Qundef)
      yield_error(compiled_schema, data, context, "required");
  }
}

static int validate_object_key(VALUE key, VALUE _value, VALUE data) {
  if(!RB_TYPE_P(key, T_STRING)) return ST_CONTINUE;

  struct memo_S *memo = (struct memo_S*)data;
  CompiledSchema *compiled_schema = memo->compiled_schema;

  ADD_TO_CONTEXT(memo->context, key);

  compiled_schema->validation_function(memo->schema, compiled_schema, key, memo->context);

  return ST_CONTINUE;
}

static void validate_property_names(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  struct memo_S memo = { schema, compiled_schema->propertyNames_schema, context };

  INCR_CONTEXT(context);

  rb_hash_foreach(data, validate_object_key, (VALUE)&memo);

  DECR_CONTEXT(context);
}

void validate_object(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!RB_TYPE_P(data, T_HASH))
    return yield_error(compiled_schema, data, context, "type_object");

  if(RB_INTEGER_TYPE_P(compiled_schema->maxProperties_val)) {
    if(RHASH_SIZE(data) > NUM2ULONG(compiled_schema->maxProperties_val))
      yield_error(compiled_schema, data, context, "maxProperties");
  }

  if(RB_INTEGER_TYPE_P(compiled_schema->minProperties_val)) {
    if(RHASH_SIZE(data) < NUM2ULONG(compiled_schema->minProperties_val))
      yield_error(compiled_schema, data, context, "minProperties");
  }

  if(compiled_schema->required_val != Qundef)
    validate_required(schema, compiled_schema, data, context);

  if(compiled_schema->properties_val != Qundef)
    validate_properties(schema, compiled_schema, data, context);

  if(compiled_schema->propertyNames_schema != NULL)
    validate_property_names(schema, compiled_schema, data, context);
}
