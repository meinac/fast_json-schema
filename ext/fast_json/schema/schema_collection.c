#include "schema_collection.h"
#include "compiled_schema.h"
#include "path.h"

extern CompiledSchema *create_compiled_schema(CompiledSchema *, VALUE, schema_flag_t);
extern void compile(CompiledSchema *, VALUE, VALUE, VALUE);

void compile_schema_collection(CompiledSchema *parent, VALUE *schema_member, VALUE ruby_schema_array, VALUE keyword, VALUE ref_data, VALUE custom_formats) {
  long i;
  VALUE compiled_schema_collection = rb_ary_new();

  /*
  * `ruby_schema_array` must be a non-empty array.
  * Perhaps we should check if it's empty.
  */
  for(i = 0; i < RARRAY_LEN(ruby_schema_array); i++) {
    VALUE ruby_schema = rb_ary_entry(ruby_schema_array, i);
    VALUE path = append_long_to_path(keyword, i);

    CompiledSchema *compiled_schema = create_compiled_schema(parent, path, EXPOSE_TO_RUBY);
    VALUE protected_path = compiled_schema->path;
    VALUE compiled_schema_obj = WrapCompiledSchema(compiled_schema);

    compile(compiled_schema, ruby_schema, ref_data, custom_formats);
    rb_ary_push(compiled_schema_collection, compiled_schema_obj);
    RB_GC_GUARD(protected_path);
  }

  *schema_member = compiled_schema_collection;
}
