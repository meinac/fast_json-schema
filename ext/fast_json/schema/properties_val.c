#include "properties_val.h"
#include "keywords.h"
#include "path.h"
#include "compiled_schema.h"

extern CompiledSchema *compile(VALUE, VALUE, VALUE, schema_flag_t);

struct memo_S {
  VALUE properties_hash;
  VALUE ref_hash;
  VALUE path;
};

static int compile_property(VALUE key, VALUE value, VALUE data) {
  if(!RB_TYPE_P(key, T_STRING)) return ST_CONTINUE;

  struct memo_S *memo = (struct memo_S*)(data);

  VALUE path = new_path(memo->path, key);

  CompiledSchema *compiled_schema = compile(value, memo->ref_hash, path, EXPOSE_TO_RUBY);
  VALUE compiled_schema_obj = WrapCompiledSchema(compiled_schema);

  rb_hash_aset(memo->properties_hash, key, compiled_schema_obj);

  return ST_CONTINUE;
}

void compile_properties_val(CompiledSchema *root_schema, VALUE ruby_schema, VALUE ref_hash, VALUE path) {
  VALUE properties = rb_hash_aref(ruby_schema, properties_str);

  if(!RB_TYPE_P(properties, T_HASH)) return;

  VALUE properties_hash = rb_hash_new();
  VALUE properties_path = new_path(path, rb_str_new2("properties"));

  struct memo_S memo = { properties_hash, ref_hash, properties_path };

  rb_hash_foreach(properties, compile_property, (VALUE)&memo);

  root_schema->properties_val = properties_hash;
}
