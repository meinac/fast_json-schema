#include "properties_val.h"
#include "keywords.h"
#include "path.h"
#include "compiled_schema.h"

extern CompiledSchema *create_compiled_schema(VALUE, schema_flag_t);
extern void compile(CompiledSchema *, VALUE, VALUE);

typedef VALUE (*key_transform_function)(VALUE);

struct memo_S {
  VALUE properties_hash;
  VALUE ref_hash;
  VALUE path;
  key_transform_function key_transform_function;
};

VALUE no_op_key_transform(VALUE key) {
  return key;
}

VALUE regexp_key_transform(VALUE key) {
  return rb_reg_new_str(key, 0);
}

static int compile_property(VALUE key, VALUE value, VALUE data) {
  if(!RB_TYPE_P(key, T_STRING)) return ST_CONTINUE;

  struct memo_S *memo = (struct memo_S*)(data);

  VALUE path = new_path(memo->path, key);

  CompiledSchema *compiled_schema = create_compiled_schema(path, EXPOSE_TO_RUBY);
  VALUE compiled_schema_obj = WrapCompiledSchema(compiled_schema);

  compile(compiled_schema, value, memo->ref_hash);

  rb_hash_aset(memo->properties_hash, memo->key_transform_function(key), compiled_schema_obj);

  return ST_CONTINUE;
}

static VALUE compile_properties(CompiledSchema *root_schema, VALUE ruby_schema, VALUE ref_hash, VALUE keyword, key_transform_function func) {
  VALUE properties = rb_hash_aref(ruby_schema, keyword);

  if(!RB_TYPE_P(properties, T_HASH)) return Qundef;

  VALUE properties_hash = rb_hash_new();
  VALUE properties_path = new_path(root_schema->path, keyword);

  struct memo_S memo = { properties_hash, ref_hash, properties_path, func };

  rb_hash_foreach(properties, compile_property, (VALUE)&memo);

  return properties_hash;
}

void compile_properties_val(CompiledSchema *root_schema, VALUE ruby_schema, VALUE ref_hash) {
  root_schema->properties_val = compile_properties(root_schema, ruby_schema, ref_hash, properties_str, no_op_key_transform);
}

void compile_pattern_properties_val(CompiledSchema *root_schema, VALUE ruby_schema, VALUE ref_hash) {
  root_schema->patternProperties_val = compile_properties(root_schema, ruby_schema, ref_hash, patternProperties_str, regexp_key_transform);
}

static void compile_array_dependency(VALUE dependencies_hash, VALUE key, VALUE value) {
  long i;

  for(i = 0; i < RARRAY_LEN(value); i++) {
    VALUE entry = rb_ary_entry(value, i);

    if(!RB_TYPE_P(entry, T_STRING)) return;
  }

  rb_hash_aset(dependencies_hash, key, rb_obj_dup(value));
}

static int compile_dependency(VALUE key, VALUE value, VALUE data) {
  if(!RB_TYPE_P(key, T_STRING)) return ST_CONTINUE;

  struct memo_S *memo = (struct memo_S *)data;

  if(RB_TYPE_P(value, T_HASH)) {
    compile_property(key, value, data);
  } else if(RB_TYPE_P(value, T_ARRAY)) {
    compile_array_dependency(memo->properties_hash, key, value);
  }

  return ST_CONTINUE;
}

void compile_dependencies_val(CompiledSchema *root_schema, VALUE ruby_schema, VALUE ref_hash) {
  VALUE dependencies = rb_hash_aref(ruby_schema, dependencies_str);

  if(!RB_TYPE_P(dependencies, T_HASH)) return;

  VALUE dependencies_val = rb_hash_new();
  VALUE dependencies_path = new_path(root_schema->path, dependencies_str);

  struct memo_S memo = { root_schema->dependencies_val, ref_hash, dependencies_path, no_op_key_transform };

  rb_hash_foreach(dependencies, compile_dependency, (VALUE)&memo);

  root_schema->dependencies_val = dependencies_val;
}
