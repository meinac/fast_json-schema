#include "compiled_schema.h"
#include "keywords.h"
#include "validate.h"
#include "path.h"
#include "value_pointer_caster.h"

#define ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_1(keyword, type)       \
  do {                                                               \
    VALUE keyword##_val  = rb_hash_aref(ruby_schema, keyword##_str); \
                                                                     \
    if(RB_TYPE_P(keyword##_val, type))                               \
      compiled_schema->keyword##_val = keyword##_val;                \
  } while(0);

#define ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_2(keyword, type_1, type_2) \
  do {                                                                   \
    VALUE keyword##_val  = rb_hash_aref(ruby_schema, keyword##_str);     \
                                                                         \
    if(RB_TYPE_P(keyword##_val, type_1) ||                               \
       RB_TYPE_P(keyword##_val, type_2))                                 \
      compiled_schema->keyword##_val = keyword##_val;                    \
  } while(0);

#define ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_3(keyword, type_1, type_2, type_3) \
  do {                                                                           \
    VALUE keyword##_val  = rb_hash_aref(ruby_schema, keyword##_str);             \
                                                                                 \
    if(RB_TYPE_P(keyword##_val, type_1) ||                                       \
       RB_TYPE_P(keyword##_val, type_2) ||                                       \
       RB_TYPE_P(keyword##_val, type_3))                                         \
      compiled_schema->keyword##_val = keyword##_val;                            \
  } while(0);

#define ASSIGN_SCHEMA_TO_COMPILED_SCHEMA(keyword)                                           \
  do {                                                                                      \
    VALUE keyword##_val  = rb_hash_aref(ruby_schema, keyword##_str);                        \
                                                                                            \
    if(!NIL_P(keyword##_val)) {                                                             \
      VALUE child_path = new_path(path, keyword##_str);                                     \
      CompiledSchema *child_schema = compile(keyword##_val, ref_hash, child_path, NO_FLAG); \
                                                                                            \
      compiled_schema->keyword##_schema = child_schema;                                     \
    }                                                                                       \
  } while(0);

VALUE compiled_schema_class;

/*
* Here I mark all the values even if they are immediate values like integers or other
* values which should be already marked by the schema hash object provided while creating
* the `FastJSON::Schema` instance but it's better to be safe than sorry.
*/
static void mark_compiled_schema(void *ptr) {
  CompiledSchema *compiled_schema = (CompiledSchema *)ptr;

  if(IS_CHILD(compiled_schema)) return;

  if(compiled_schema->id_val != Qundef) rb_gc_mark(compiled_schema->id_val);
  if(compiled_schema->ref_val != Qundef) rb_gc_mark(compiled_schema->ref_val);
  if(compiled_schema->recursiveAnchor_val != Qundef) rb_gc_mark(compiled_schema->recursiveAnchor_val);
  if(compiled_schema->recursiveRef_val != Qundef) rb_gc_mark(compiled_schema->recursiveRef_val);

  if(compiled_schema->multipleOf_val != Qundef) rb_gc_mark(compiled_schema->multipleOf_val);
  if(compiled_schema->maximum_val != Qundef) rb_gc_mark(compiled_schema->maximum_val);
  if(compiled_schema->exclusiveMaximum_val != Qundef) rb_gc_mark(compiled_schema->exclusiveMaximum_val);
  if(compiled_schema->minimum_val != Qundef) rb_gc_mark(compiled_schema->minimum_val);
  if(compiled_schema->exclusiveMinimum_val != Qundef) rb_gc_mark(compiled_schema->exclusiveMinimum_val);

  if(compiled_schema->maxLength_val != Qundef) rb_gc_mark(compiled_schema->maxLength_val);
  if(compiled_schema->minLength_val != Qundef) rb_gc_mark(compiled_schema->minLength_val);
  if(compiled_schema->pattern_val != Qundef) rb_gc_mark(compiled_schema->pattern_val);

  if(compiled_schema->maxItems_val != Qundef) rb_gc_mark(compiled_schema->maxItems_val);
  if(compiled_schema->minItems_val != Qundef) rb_gc_mark(compiled_schema->minItems_val);
  if(compiled_schema->uniqueItems_val != Qundef) rb_gc_mark(compiled_schema->uniqueItems_val);
  if(compiled_schema->maxContains_val != Qundef) rb_gc_mark(compiled_schema->maxContains_val);
  if(compiled_schema->minContains_val != Qundef) rb_gc_mark(compiled_schema->minContains_val);

  if(compiled_schema->maxProperties_val != Qundef) rb_gc_mark(compiled_schema->maxProperties_val);
  if(compiled_schema->minProperties_val != Qundef) rb_gc_mark(compiled_schema->minProperties_val);
  if(compiled_schema->required_val != Qundef) rb_gc_mark(compiled_schema->required_val);
  if(compiled_schema->dependentRequired_val != Qundef) rb_gc_mark(compiled_schema->dependentRequired_val);
}

static void free_child_schema(CompiledSchema *compiled_schema) {
  if(compiled_schema->items_schema != NULL) free_child_schema(compiled_schema->items_schema);
  if(compiled_schema->contains_schema != NULL) free_child_schema(compiled_schema->contains_schema);
  if(compiled_schema->properties_schema != NULL) free_child_schema(compiled_schema->properties_schema);

  xfree(compiled_schema);
}

/*
* This gets called by the Ruby GC for each `FastJSON::Schema::CompiledSchema` instance.
* As we are also wrapping the child compiled schemas into Ruby instances to pass them between
* Ruby methods, the pointer we receive here can refer to a child compiled schema which is needed
* by its parent. In that case, we shouldn't free the memory block addressed by that pointer and
* let the root compiled schema to handle freeing them.
*/
static void free_compiled_schema(void *ptr) {
  CompiledSchema *compiled_schema = (CompiledSchema *)ptr;

  if(IS_CHILD(compiled_schema)) return;

  if(compiled_schema->items_schema != NULL) free_child_schema(compiled_schema->items_schema);
  if(compiled_schema->contains_schema != NULL) free_child_schema(compiled_schema->contains_schema);
  if(compiled_schema->properties_schema != NULL) free_child_schema(compiled_schema->properties_schema);

  xfree(compiled_schema);
}

/*
* Here I am re-assigning all the values including the immediate ones which can not be
* compacted as they are located in stack but there is no harm re-assigning them.
*/
static void compact_compiled_schema(void *ptr) {
  CompiledSchema *compiled_schema = (CompiledSchema *)ptr;

  if(IS_CHILD(compiled_schema)) return;

  if(compiled_schema->id_val != Qundef) compiled_schema->id_val = rb_gc_location(compiled_schema->id_val);
  if(compiled_schema->ref_val != Qundef) compiled_schema->ref_val = rb_gc_location(compiled_schema->ref_val);
  if(compiled_schema->recursiveAnchor_val != Qundef) compiled_schema->recursiveAnchor_val = rb_gc_location(compiled_schema->recursiveAnchor_val);
  if(compiled_schema->recursiveRef_val != Qundef) compiled_schema->recursiveRef_val = rb_gc_location(compiled_schema->recursiveRef_val);

  if(compiled_schema->multipleOf_val != Qundef) compiled_schema->multipleOf_val = rb_gc_location(compiled_schema->multipleOf_val);
  if(compiled_schema->maximum_val != Qundef) compiled_schema->maximum_val = rb_gc_location(compiled_schema->maximum_val);
  if(compiled_schema->exclusiveMaximum_val != Qundef) compiled_schema->exclusiveMaximum_val = rb_gc_location(compiled_schema->exclusiveMaximum_val);
  if(compiled_schema->minimum_val != Qundef) compiled_schema->minimum_val = rb_gc_location(compiled_schema->minimum_val);
  if(compiled_schema->exclusiveMinimum_val != Qundef) compiled_schema->exclusiveMinimum_val = rb_gc_location(compiled_schema->exclusiveMinimum_val);

  if(compiled_schema->maxLength_val != Qundef) compiled_schema->maxLength_val = rb_gc_location(compiled_schema->maxLength_val);
  if(compiled_schema->minLength_val != Qundef) compiled_schema->minLength_val = rb_gc_location(compiled_schema->minLength_val);
  if(compiled_schema->pattern_val != Qundef) compiled_schema->pattern_val = rb_gc_location(compiled_schema->pattern_val);

  if(compiled_schema->maxItems_val != Qundef) compiled_schema->maxItems_val = rb_gc_location(compiled_schema->maxItems_val);
  if(compiled_schema->minItems_val != Qundef) compiled_schema->minItems_val = rb_gc_location(compiled_schema->minItems_val);
  if(compiled_schema->uniqueItems_val != Qundef) compiled_schema->uniqueItems_val = rb_gc_location(compiled_schema->uniqueItems_val);
  if(compiled_schema->maxContains_val != Qundef) compiled_schema->maxContains_val = rb_gc_location(compiled_schema->maxContains_val);
  if(compiled_schema->minContains_val != Qundef) compiled_schema->minContains_val = rb_gc_location(compiled_schema->minContains_val);

  if(compiled_schema->maxProperties_val != Qundef) compiled_schema->maxProperties_val = rb_gc_location(compiled_schema->maxProperties_val);
  if(compiled_schema->minProperties_val != Qundef) compiled_schema->minProperties_val = rb_gc_location(compiled_schema->minProperties_val);
  if(compiled_schema->required_val != Qundef) compiled_schema->required_val = rb_gc_location(compiled_schema->required_val);
  if(compiled_schema->dependentRequired_val != Qundef) compiled_schema->dependentRequired_val = rb_gc_location(compiled_schema->dependentRequired_val);
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

  compiled_schema->multipleOf_val = Qundef;
  compiled_schema->maximum_val = Qundef;
  compiled_schema->exclusiveMaximum_val = Qundef;
  compiled_schema->minimum_val = Qundef;
  compiled_schema->exclusiveMinimum_val = Qundef;

  compiled_schema->maxLength_val = Qundef;
  compiled_schema->minLength_val = Qundef;
  compiled_schema->pattern_val = Qundef;

  compiled_schema->items_schema = NULL;
  compiled_schema->contains_schema = NULL;
  compiled_schema->maxItems_val = Qundef;
  compiled_schema->minItems_val = Qundef;
  compiled_schema->uniqueItems_val = Qundef;
  compiled_schema->maxContains_val = Qundef;
  compiled_schema->minContains_val = Qundef;

  compiled_schema->properties_schema = NULL;
  compiled_schema->maxProperties_val = Qundef;
  compiled_schema->minProperties_val = Qundef;
  compiled_schema->required_val = Qundef;
  compiled_schema->dependentRequired_val = Qundef;

  return compiled_schema;
}

static validation_function type_validation_function(VALUE ruby_schema) {
  VALUE type_val = rb_hash_aref(ruby_schema, type_str);

  if(!RB_TYPE_P(type_val, T_STRING)) return no_op_validate;

  char *type_str = StringValuePtr(type_val);

  if(strcmp(type_str, "null") == 0) {
    return validate_null;
  } else if(strcmp(type_str, "boolean") == 0) {
    return validate_bool;
  } else if(strcmp(type_str, "string") == 0) {
    return validate_string;
  } else if(strcmp(type_str, "integer") == 0) {
    return validate_integer;
  } else if(strcmp(type_str, "number") == 0) {
    return validate_number;
  } else if(strcmp(type_str, "array") == 0) {
    return validate_array;
  } else if(strcmp(type_str, "object") == 0) {
    return validate_object;
  }

  return no_op_validate;
}

static CompiledSchema *compile(VALUE ruby_schema, VALUE ref_hash, VALUE path, schema_flag_t flags) {
  CompiledSchema *compiled_schema = create_compiled_schema(path);

  compiled_schema->flags = flags;

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

  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_1(id, T_STRING);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_1(ref, T_STRING);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_1(recursiveAnchor, T_STRING);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_1(recursiveRef, T_STRING);

  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_3(multipleOf, T_FIXNUM, T_BIGNUM, T_FLOAT);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_3(maximum, T_FIXNUM, T_BIGNUM, T_FLOAT);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_3(exclusiveMaximum, T_FIXNUM, T_BIGNUM, T_FLOAT);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_3(minimum, T_FIXNUM, T_BIGNUM, T_FLOAT);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_3(exclusiveMinimum, T_FIXNUM, T_BIGNUM, T_FLOAT);

  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_2(maxLength, T_FIXNUM, T_BIGNUM);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_2(minLength, T_FIXNUM, T_BIGNUM);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_1(pattern, T_STRING);

  ASSIGN_SCHEMA_TO_COMPILED_SCHEMA(items);
  ASSIGN_SCHEMA_TO_COMPILED_SCHEMA(contains);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_2(maxItems, T_FIXNUM, T_BIGNUM);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_2(minItems, T_FIXNUM, T_BIGNUM);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_2(uniqueItems, T_TRUE, T_FALSE);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_2(maxContains, T_FIXNUM, T_BIGNUM);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_2(minContains, T_FIXNUM, T_BIGNUM);

  ASSIGN_SCHEMA_TO_COMPILED_SCHEMA(properties);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_2(maxProperties, T_FIXNUM, T_BIGNUM);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_2(minProperties, T_FIXNUM, T_BIGNUM);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_1(required, T_ARRAY);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_1(dependentRequired, T_HASH);

  compiled_schema->type_validation_function = type_validation_function(ruby_schema);

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

  CompiledSchema *compiled_schema = compile(ruby_schema, ref_hash, root_path_str, ROOT_SCHEMA);
  VALUE compiled_schema_obj = TypedData_Wrap_Struct(compiled_schema_class, &compiled_schema_type, compiled_schema);

  rb_ivar_set(self, rb_intern("compiled_schema"), compiled_schema_obj);
  rb_ivar_set(self, rb_intern("compiled"), Qtrue);
}

void Init_compiled_schema(VALUE schema_class) {
  compiled_schema_class = rb_define_class_under(schema_class, "CompiledSchema", rb_cObject);

  rb_define_alloc_func(compiled_schema_class, alloc_compiled_schema);
}
