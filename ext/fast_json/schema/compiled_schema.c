#include "compiled_schema.h"
#include "keywords.h"
#include "validate.h"

#if SIZEOF_VOIDP == SIZEOF_LONG
# define PTR2NUM(x)   (LONG2NUM((long)(x)))
# define NUM2PTR(x)   ((void *)(NUM2ULONG(x)))
#else
# define PTR2NUM(x)   (LL2NUM((LONG_LONG)(x)))
# define NUM2PTR(x)   ((void *)(NUM2ULL(x)))
#endif

#define ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA(keyword, type)         \
  do {                                                               \
    VALUE keyword##_val  = rb_hash_aref(ruby_schema, keyword##_str); \
                                                                     \
    if(RB_TYPE_P(keyword##_val, type))                               \
      compiled_schema->keyword##_val = keyword##_val;                \
  } while(0);

VALUE compiled_schema_class;

static void mark_compiled_schema(void *ptr) {
  CompiledSchema *compiled_schema = (CompiledSchema *)ptr;

  if(compiled_schema->id_val != Qundef) rb_gc_mark(compiled_schema->id_val);
  if(compiled_schema->ref_val != Qundef) rb_gc_mark(compiled_schema->ref_val);
  if(compiled_schema->recursiveAnchor_val != Qundef) rb_gc_mark(compiled_schema->recursiveAnchor_val);
  if(compiled_schema->recursiveRef_val != Qundef) rb_gc_mark(compiled_schema->recursiveRef_val);
}

static void free_compiled_schema(void *ptr) {
  xfree(ptr);
}

static void compact_compiled_schema(void *ptr) {
  CompiledSchema *compiled_schema = (CompiledSchema *)ptr;

  if(compiled_schema->id_val != Qundef) compiled_schema->id_val = rb_gc_location(compiled_schema->id_val);
  if(compiled_schema->ref_val != Qundef) compiled_schema->ref_val = rb_gc_location(compiled_schema->ref_val);
  if(compiled_schema->recursiveAnchor_val != Qundef) compiled_schema->recursiveAnchor_val = rb_gc_location(compiled_schema->recursiveAnchor_val);
  if(compiled_schema->recursiveRef_val != Qundef) compiled_schema->recursiveRef_val = rb_gc_location(compiled_schema->recursiveRef_val);
}

const rb_data_type_t compiled_schema_type = {
  "Schema::CompiledSchema",
  {
    mark_compiled_schema,
    free_compiled_schema,
    0,
    compact_compiled_schema
  },
  0,
  0,
  0,
};

static VALUE alloc_compiled_schema(VALUE klass) {
  CompiledSchema *compiled_schema;
  VALUE object = TypedData_Make_Struct(klass, CompiledSchema, &compiled_schema_type, compiled_schema);

  return object;
}

static CompiledSchema *create_compiled_schema(VALUE path) {
  CompiledSchema *compiled_schema = ALLOC(CompiledSchema);

  compiled_schema->path = path;

  compiled_schema->id_val = Qundef;
  compiled_schema->ref_val = Qundef;
  compiled_schema->recursiveAnchor_val = Qundef;
  compiled_schema->recursiveRef_val = Qundef;

  return compiled_schema;
}

static CompiledSchema *compile(VALUE ruby_schema, VALUE ref_hash, VALUE path) {
  CompiledSchema *compiled_schema = create_compiled_schema(path);

  if(ruby_schema == Qfalse)
    compiled_schema->validation_function = false_validate;

  if(ruby_schema == Qtrue)
    compiled_schema->validation_function = no_op_validate;

  if(!RB_TYPE_P(ruby_schema, T_HASH))
    return compiled_schema;

  if(RHASH_SIZE(ruby_schema) == 0) {
    compiled_schema->validation_function = no_op_validate;

    return compiled_schema;
  }

  compiled_schema->validation_function = validate;

  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA(id, T_STRING);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA(ref, T_STRING);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA(recursiveAnchor, T_STRING);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA(recursiveRef, T_STRING);

  // Embed compiled schema into Ruby Hash
  rb_hash_aset(ref_hash, path, PTR2NUM(compiled_schema));

  // Embed compiled schema into Ruby Hash again if it has an $id
  if(compiled_schema->id_val != Qundef)
    rb_hash_aset(ref_hash, compiled_schema->id_val, PTR2NUM(compiled_schema));

  return compiled_schema;
}

void compile_schema(VALUE self) {
  VALUE ruby_schema = rb_ivar_get(self, rb_intern("@ruby_schema"));
  VALUE ref_hash = rb_hash_new();

  CompiledSchema *compiled_schema = compile(ruby_schema, ref_hash, root_path_str);
  VALUE compiled_schema_obj = TypedData_Wrap_Struct(compiled_schema_class, &compiled_schema_type, compiled_schema);

  rb_ivar_set(self, rb_intern("compiled_schema"), compiled_schema_obj);
  rb_ivar_set(self, rb_intern("compiled"), Qtrue);
}

void Init_compiled_schema(VALUE schema_class) {
  compiled_schema_class = rb_define_class_under(schema_class, "CompiledSchema", rb_cObject);

  rb_define_alloc_func(compiled_schema_class, alloc_compiled_schema);
}
