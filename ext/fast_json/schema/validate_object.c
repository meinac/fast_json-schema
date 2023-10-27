#include "validate_object.h"
#include "error.h"

struct properties_memo_S {
  VALUE schema;
  CompiledSchema *compiled_schema;
  Context *context;
  bool validated;
};

struct pattern_properties_memo_S {
  VALUE schema;
  Context *context;
  VALUE key;
  VALUE value;
  bool validated;
};

struct dependencies_memo_S {
  VALUE schema;
  VALUE data;
  Context *context;
};

static int validate_property_by_pattern(VALUE regexp, VALUE compiled_schema_obj, VALUE data) {
  struct pattern_properties_memo_S *memo = (struct pattern_properties_memo_S *)data;

  VALUE match = rb_reg_match(regexp, memo->key);

  if(NIL_P(match)) return ST_CONTINUE;

  CompiledSchema *compiled_schema;
  GetCompiledSchema(compiled_schema_obj, compiled_schema);

  compiled_schema->validation_function(memo->schema, compiled_schema, memo->value, memo->context);

  memo->validated = true;

  return ST_STOP;
}

static void validate_by_pattern_properties_keyword(struct properties_memo_S *memo, VALUE key, VALUE value) {
  VALUE patternProperties_val = memo->compiled_schema->patternProperties_val;
  struct pattern_properties_memo_S pattern_memo = { memo->schema, memo->context, key, value, false };

  rb_hash_foreach(patternProperties_val, validate_property_by_pattern, (VALUE)&pattern_memo);

  memo->validated |= pattern_memo.validated;
}

static void validate_by_properties_keyword(struct properties_memo_S *memo, VALUE key, VALUE value) {
  VALUE properties_val = memo->compiled_schema->properties_val;
  VALUE compiled_schema_obj = rb_hash_aref(properties_val, key);

  if(NIL_P(compiled_schema_obj)) return;

  CompiledSchema *compiled_schema;
  GetCompiledSchema(compiled_schema_obj, compiled_schema);

  compiled_schema->validation_function(memo->schema, compiled_schema, value, memo->context);

  memo->validated = true;
}

static int validate_object_property(VALUE key, VALUE value, VALUE data) {
  if(!RB_TYPE_P(key, T_STRING)) return ST_CONTINUE;

  struct properties_memo_S *memo = (struct properties_memo_S*)data;

  VALUE properties_val = memo->compiled_schema->properties_val;
  VALUE patternProperties_val = memo->compiled_schema->patternProperties_val;
  CompiledSchema *propertyNames_schema = memo->compiled_schema->propertyNames_schema;
  CompiledSchema *additionalProperties_schema = memo->compiled_schema->additionalProperties_schema;

  ADD_TO_CONTEXT(memo->context, key);

  memo->validated = false;

  if(propertyNames_schema != NULL)
    propertyNames_schema->validation_function(memo->schema, propertyNames_schema, key, memo->context);

  if(properties_val != Qundef)
    validate_by_properties_keyword(memo, key, value);

  if(patternProperties_val != Qundef)
    validate_by_pattern_properties_keyword(memo, key, value);

  if(additionalProperties_schema != NULL && !memo->validated)
    additionalProperties_schema->validation_function(memo->schema, additionalProperties_schema, value, memo->context);

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

static int validate_dependency(VALUE key, VALUE value, VALUE data) {
  struct dependencies_memo_S *memo = (struct dependencies_memo_S *)data;

  VALUE hash_value = rb_hash_lookup2(memo->data, key, Qundef);

  if(hash_value != Qundef) {
    CompiledSchema *dependency_schema;
    GetCompiledSchema(value, dependency_schema);

    dependency_schema->validation_function(memo->schema, dependency_schema, memo->data, memo->context);
  }

  return ST_CONTINUE;
}

static void validate_dependencies(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  struct dependencies_memo_S memo = { schema, data, context };

  rb_hash_foreach(compiled_schema->dependencies_val, validate_dependency, (VALUE)&memo);
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

  if(compiled_schema->dependencies_val != Qundef)
    validate_dependencies(schema, compiled_schema, data, context);


  validate_properties(schema, compiled_schema, data, context);
}
