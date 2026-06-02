#include "properties_val.h"
#include "keywords.h"
#include "path.h"
#include "compiled_schema.h"

extern CompiledSchema *create_compiled_schema(CompiledSchema *, VALUE, schema_flag_t);
extern void compile(CompiledSchema *, VALUE, CompileContext *);

typedef VALUE (*key_transform_function)(VALUE);

struct memo_S {
  CompiledSchema *parent;
  VALUE properties_hash;
  VALUE keyword;
  CompileContext *ctx;
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

  VALUE path = new_path(memo->keyword, key);

  CompiledSchema *compiled_schema = create_compiled_schema(memo->parent, path, EXPOSE_TO_RUBY);
  VALUE protected_path = compiled_schema->path;
  VALUE compiled_schema_obj = WrapCompiledSchema(compiled_schema);

  compile(compiled_schema, value, memo->ctx);

  rb_hash_aset(memo->properties_hash, memo->key_transform_function(key), compiled_schema_obj);

  RB_GC_GUARD(protected_path);

  return ST_CONTINUE;
}

static VALUE compile_properties(CompiledSchema *root_schema, VALUE ruby_schema, CompileContext *ctx, VALUE keyword, key_transform_function func) {
  VALUE properties = rb_hash_aref(ruby_schema, keyword);

  if(!RB_TYPE_P(properties, T_HASH)) return Qundef;

  VALUE properties_hash = rb_hash_new();

  struct memo_S memo = { root_schema, properties_hash, keyword, ctx, func };

  rb_hash_foreach(properties, compile_property, (VALUE)&memo);

  return properties_hash;
}

void compile_properties_val(CompiledSchema *root_schema, VALUE ruby_schema, CompileContext *ctx) {
  root_schema->properties_val = compile_properties(root_schema, ruby_schema, ctx, properties_str, no_op_key_transform);
}

void compile_pattern_properties_val(CompiledSchema *root_schema, VALUE ruby_schema, CompileContext *ctx) {
  root_schema->patternProperties_val = compile_properties(root_schema, ruby_schema, ctx, patternProperties_str, regexp_key_transform);
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

void compile_dependencies_val(CompiledSchema *root_schema, VALUE ruby_schema, CompileContext *ctx) {
  VALUE dependencies = rb_hash_aref(ruby_schema, dependencies_str);

  if(!RB_TYPE_P(dependencies, T_HASH)) return;

  VALUE dependencies_val = rb_hash_new();

  struct memo_S memo = { root_schema, dependencies_val, dependencies_str, ctx, no_op_key_transform };

  rb_hash_foreach(dependencies, compile_dependency, (VALUE)&memo);

  root_schema->dependencies_val = dependencies_val;
}
