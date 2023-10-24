#include "validate_object.h"
#include "error.h"

struct properties_memo_S {
  VALUE schema;
  CompiledSchema *compiled_schema;
  Context *context;
};

struct pattern_properties_memo_S {
  VALUE schema;
  Context *context;
  VALUE key;
  VALUE value;
};

static int validate_property_by_pattern(VALUE regexp, VALUE compiled_schema_obj, VALUE data) {
  struct pattern_properties_memo_S *memo = (struct pattern_properties_memo_S *)data;

  VALUE match = rb_reg_match(regexp, memo->key);

  if(NIL_P(match)) return ST_CONTINUE;

  CompiledSchema *compiled_schema;
  GetCompiledSchema(compiled_schema_obj, compiled_schema);

  compiled_schema->validation_function(memo->schema, compiled_schema, memo->value, memo->context);

  return ST_STOP;
}

static void validate_by_pattern_properties_keyword(VALUE schema, VALUE pattern_properties_val, VALUE key, VALUE value, Context *context) {
  struct pattern_properties_memo_S memo = { schema, context, key, value };

  rb_hash_foreach(pattern_properties_val, validate_property_by_pattern, (VALUE)&memo);
}

static void validate_by_properties_keyword(VALUE schema, VALUE properties_val, VALUE key, VALUE value, Context *context) {
  VALUE compiled_schema_obj = rb_hash_aref(properties_val, key);

  if(NIL_P(compiled_schema_obj)) return;

  CompiledSchema *compiled_schema;
  GetCompiledSchema(compiled_schema_obj, compiled_schema);

  compiled_schema->validation_function(schema, compiled_schema, value, context);
}

static int validate_object_property(VALUE key, VALUE value, VALUE data) {
  if(!RB_TYPE_P(key, T_STRING)) return ST_CONTINUE;

  struct properties_memo_S *memo = (struct properties_memo_S*)data;

  VALUE properties_val = memo->compiled_schema->properties_val;
  VALUE patternProperties_val = memo->compiled_schema->patternProperties_val;
  CompiledSchema *propertyNames_schema = memo->compiled_schema->propertyNames_schema;

  ADD_TO_CONTEXT(memo->context, key);

  if(propertyNames_schema != NULL)
    propertyNames_schema->validation_function(memo->schema, propertyNames_schema, key, memo->context);

  if(properties_val != Qundef)
    validate_by_properties_keyword(memo->schema, properties_val, key, value, memo->context);

  if(patternProperties_val != Qundef)
    validate_by_pattern_properties_keyword(memo->schema, patternProperties_val, key, value, memo->context);

  return ST_CONTINUE;
}

static void validate_properties(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  struct properties_memo_S memo = { schema, compiled_schema, context };

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

  validate_properties(schema, compiled_schema, data, context);
}
