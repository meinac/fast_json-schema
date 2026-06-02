#include "nested_schemas.h"
#include "keywords.h"
#include "path.h"

extern CompiledSchema *create_compiled_schema(CompiledSchema *, VALUE, schema_flag_t);
extern void compile(CompiledSchema *, VALUE, CompileContext *);

static bool is_schema_shaped(VALUE value) {
  return RB_TYPE_P(value, T_HASH) ||
         RB_TYPE_P(value, T_TRUE) ||
         RB_TYPE_P(value, T_FALSE);
}

static size_t count_schema_entries_in_array(VALUE array) {
  size_t count = 0;

  for(long i = 0; i < RARRAY_LEN(array); i++) {
    VALUE entry = rb_ary_entry(array, i);

    if(is_schema_shaped(entry)) {
      count++;
    } else if(RB_TYPE_P(entry, T_ARRAY)) {
      count += count_schema_entries_in_array(entry);
    }
  }

  return count;
}

static int count_nested(VALUE key, VALUE value, VALUE data) {
  if(!RB_TYPE_P(key, T_STRING)) return ST_CONTINUE;
  if(is_known_keyword(key)) return ST_CONTINUE;

  size_t *count = (size_t *)data;

  if(is_schema_shaped(value)) {
    (*count)++;
  } else if(RB_TYPE_P(value, T_ARRAY)) {
    *count += count_schema_entries_in_array(value);
  }

  return ST_CONTINUE;
}

struct compile_memo_S {
  CompiledSchema *root_schema;
  CompileContext *ctx;
};

static void compile_array_entries(VALUE array_path, VALUE array, struct compile_memo_S *memo);

static void compile_entry(VALUE value, VALUE path, struct compile_memo_S *memo) {
  if(is_schema_shaped(value)) {
    CompiledSchema *child = create_compiled_schema(memo->root_schema, path, NO_FLAG);

    /*
    * Append the child pointer to the parent's nested_schemas array and bump the
    * count BEFORE running `compile` on the child, so the parent's mark function
    * will reach this child if the GC fires during its compilation.
    */
    memo->root_schema->nested_schemas[memo->root_schema->nested_schemas_count++] = child;

    compile(child, value, memo->ctx);
  } else if(RB_TYPE_P(value, T_ARRAY)) {
    compile_array_entries(path, value, memo);
  }
}

static void compile_array_entries(VALUE array_path, VALUE array, struct compile_memo_S *memo) {
  for(long i = 0; i < RARRAY_LEN(array); i++) {
    VALUE entry = rb_ary_entry(array, i);
    VALUE entry_path = append_long_to_path(array_path, i);

    compile_entry(entry, entry_path, memo);
  }
}

static int compile_nested(VALUE key, VALUE value, VALUE data) {
  if(!RB_TYPE_P(key, T_STRING)) return ST_CONTINUE;
  if(is_known_keyword(key)) return ST_CONTINUE;

  compile_entry(value, key, (struct compile_memo_S *)data);

  return ST_CONTINUE;
}

void compile_nested_schemas(CompiledSchema *root_schema, VALUE ruby_schema, CompileContext *ctx) {
  if(!RB_TYPE_P(ruby_schema, T_HASH)) return;

  size_t total = 0;
  rb_hash_foreach(ruby_schema, count_nested, (VALUE)&total);

  if(total == 0) return;

  root_schema->nested_schemas = ALLOC_N(CompiledSchema *, total);
  root_schema->nested_schemas_count = 0;

  struct compile_memo_S memo = { root_schema, ctx };

  rb_hash_foreach(ruby_schema, compile_nested, (VALUE)&memo);
}
