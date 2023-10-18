#include "schema_collection.h"
#include "compiled_schema.h"
#include "path.h"

extern CompiledSchema *compile(VALUE, VALUE, VALUE, schema_flag_t);

void compile_schema_collection(VALUE *schema_member, VALUE ruby_schema_array, VALUE ref_hash, VALUE path) {
  long i;
  VALUE compiled_schema_collection = rb_ary_new();

  /*
  * `ruby_schema_array` must be a non-empty array.
  * Perhaps we should check if it's empty.
  */
  for(i = 0; i < RARRAY_LEN(ruby_schema_array); i++) {
    VALUE ruby_schema = rb_ary_entry(ruby_schema_array, i);
    VALUE new_path = append_long_to_path(path, i);

    CompiledSchema *compiled_schema = compile(ruby_schema, ref_hash, new_path, EXPOSE_TO_RUBY);
    VALUE compiled_schema_obj = WrapCompiledSchema(compiled_schema);

    rb_ary_push(compiled_schema_collection, compiled_schema_obj);
  }

  *schema_member = compiled_schema_collection;
}
