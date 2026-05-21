#include "nested_schemas.h"
#include "keywords.h"

extern CompiledSchema *create_compiled_schema(CompiledSchema *, VALUE, schema_flag_t);
extern void compile(CompiledSchema *, VALUE, VALUE, VALUE);

static bool is_schema_shaped(VALUE value) {
  return RB_TYPE_P(value, T_HASH) ||
         RB_TYPE_P(value, T_TRUE) ||
         RB_TYPE_P(value, T_FALSE);
}

static int count_nested(VALUE key, VALUE value, VALUE data) {
  if(!RB_TYPE_P(key, T_STRING)) return ST_CONTINUE;
  if(is_known_keyword(key)) return ST_CONTINUE;
  if(!is_schema_shaped(value)) return ST_CONTINUE;

  size_t *count = (size_t *)data;
  (*count)++;

  return ST_CONTINUE;
}

struct compile_memo_S {
  CompiledSchema *root_schema;
  VALUE ref_data;
  VALUE custom_formats;
};

static int compile_nested(VALUE key, VALUE value, VALUE data) {
  if(!RB_TYPE_P(key, T_STRING)) return ST_CONTINUE;
  if(is_known_keyword(key)) return ST_CONTINUE;
  if(!is_schema_shaped(value)) return ST_CONTINUE;

  struct compile_memo_S *memo = (struct compile_memo_S *)data;

  CompiledSchema *child = create_compiled_schema(memo->root_schema, key, NO_FLAG);

  /*
  * Append the child pointer to the parent's nested_schemas array and bump the
  * count BEFORE running `compile` on the child, so the parent's mark function
  * will reach this child if the GC fires during its compilation.
  */
  memo->root_schema->nested_schemas[memo->root_schema->nested_schemas_count++] = child;

  compile(child, value, memo->ref_data, memo->custom_formats);

  return ST_CONTINUE;
}

void compile_nested_schemas(CompiledSchema *root_schema, VALUE ruby_schema, VALUE ref_data, VALUE custom_formats) {
  if(!RB_TYPE_P(ruby_schema, T_HASH)) return;

  size_t total = 0;
  rb_hash_foreach(ruby_schema, count_nested, (VALUE)&total);

  if(total == 0) return;

  root_schema->nested_schemas = ALLOC_N(CompiledSchema *, total);
  root_schema->nested_schemas_count = 0;

  struct compile_memo_S memo = { root_schema, ref_data, custom_formats };

  rb_hash_foreach(ruby_schema, compile_nested, (VALUE)&memo);
}
