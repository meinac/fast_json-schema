#include "properties_val.h"
#include "keywords.h"
#include "path.h"
#include "compiled_schema.h"

extern CompiledSchema *create_compiled_schema(VALUE, schema_flag_t);
extern void compile(CompiledSchema *, VALUE, VALUE);

typedef VALUE (*key_transform_function)(VALUE);

struct memo_S {
  VALUE properties_hash;
  VALUE ref_data;
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

  compile(compiled_schema, value, memo->ref_data);

  rb_hash_aset(memo->properties_hash, memo->key_transform_function(key), compiled_schema_obj);

  return ST_CONTINUE;
}

static VALUE compile_properties(CompiledSchema *root_schema, VALUE ruby_schema, VALUE ref_data, VALUE keyword, key_transform_function func) {
  VALUE properties = rb_hash_aref(ruby_schema, keyword);

  if(!RB_TYPE_P(properties, T_HASH)) return Qundef;

  VALUE properties_hash = rb_hash_new();
  VALUE properties_path = new_path(root_schema->path, keyword);

  struct memo_S memo = { properties_hash, ref_data, properties_path, func };

  rb_hash_foreach(properties, compile_property, (VALUE)&memo);

  return properties_hash;
}

void compile_properties_val(CompiledSchema *root_schema, VALUE ruby_schema, VALUE ref_data) {
  root_schema->properties_val = compile_properties(root_schema, ruby_schema, ref_data, properties_str, no_op_key_transform);
}

void compile_pattern_properties_val(CompiledSchema *root_schema, VALUE ruby_schema, VALUE ref_data) {
  root_schema->patternProperties_val = compile_properties(root_schema, ruby_schema, ref_data, patternProperties_str, regexp_key_transform);
}

static int compile_dependency(VALUE key, VALUE value, VALUE data) {
  if(!RB_TYPE_P(key, T_STRING)) return ST_CONTINUE;

  if(RB_TYPE_P(value, T_ARRAY)) {
    /*
    * If the value is an array, we are creating a temporary Ruby hash and compiling it as
    * if it was provided by the user which looks something like the following;
    *
    *   { "type" => "object", "required" => ["foo", "bar"] }
    */
    VALUE tmp_schema = rb_hash_new();

    rb_hash_aset(tmp_schema, required_str, value);
    rb_hash_aset(tmp_schema, type_str, object_str);

    compile_property(key, tmp_schema, data);
  } else {
    compile_property(key, value, data);
  }

  return ST_CONTINUE;
}

void compile_dependencies_val(CompiledSchema *root_schema, VALUE ruby_schema, VALUE ref_data) {
  VALUE dependencies = rb_hash_aref(ruby_schema, dependencies_str);

  if(!RB_TYPE_P(dependencies, T_HASH)) return;

  VALUE dependencies_val = rb_hash_new();
  VALUE dependencies_path = new_path(root_schema->path, dependencies_str);

  struct memo_S memo = { dependencies_val, ref_data, dependencies_path, no_op_key_transform };

  rb_hash_foreach(dependencies, compile_dependency, (VALUE)&memo);

  root_schema->dependencies_val = dependencies_val;
}
