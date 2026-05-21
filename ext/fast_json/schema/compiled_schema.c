#include "compiled_schema.h"
#include "keywords.h"
#include "validate.h"
#include "path.h"
#include "value_pointer_caster.h"
#include "properties_val.h"
#include "schema_collection.h"
#include "ref.h"
#include "ref_resolver.h"
#include "nested_schemas.h"
#include "formats/format.h"

#define ASSIGN_ANY_VALUE_TO_COMPILED_SCHEMA(keyword)                           \
  do {                                                                         \
    VALUE keyword##_val = rb_hash_lookup2(ruby_schema, keyword##_str, Qundef); \
                                                                               \
    if(keyword##_val != Qundef)                                                \
      compiled_schema->keyword##_val = keyword##_val;                          \
  } while(0);

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

#define ASSIGN_SCHEMA_TO_COMPILED_SCHEMA(keyword)                                                     \
  do {                                                                                                \
    VALUE keyword##_val  = rb_hash_aref(ruby_schema, keyword##_str);                                  \
                                                                                                      \
    if(RB_TYPE_P(keyword##_val, T_HASH) ||                                                            \
       RB_TYPE_P(keyword##_val, T_FALSE) ||                                                           \
       RB_TYPE_P(keyword##_val, T_TRUE))                                                              \
    {                                                                                                 \
      CompiledSchema *child_schema = create_compiled_schema(compiled_schema, keyword##_str, NO_FLAG); \
      compiled_schema->keyword##_schema = child_schema;                                               \
                                                                                                      \
      compile(child_schema, keyword##_val, ref_data, custom_formats);                                 \
    }                                                                                                 \
  } while(0);

#define ASSIGN_SCHEMA_COLLECTION_TO_COMPILED_SCHEMA(keyword)                                                                                 \
  do {                                                                                                                                       \
    VALUE keyword##_val = rb_hash_lookup2(ruby_schema, keyword##_str, Qundef);                                                               \
                                                                                                                                             \
    if(RB_TYPE_P(keyword##_val, T_ARRAY)) {                                                                                                  \
      compile_schema_collection(compiled_schema, &(compiled_schema->keyword##_val), keyword##_val, keyword##_str, ref_data, custom_formats); \
    }                                                                                                                                        \
  } while(0);

#define COMPACT_VALUE(keyword)                                                         \
  do {                                                                                 \
    if(compiled_schema->keyword##_val != Qundef)                                       \
      compiled_schema->keyword##_val = rb_gc_location(compiled_schema->keyword##_val); \
  } while(0);

#define COMPACT_CHILD_SCHEMA(keyword)                             \
  do {                                                            \
    if(compiled_schema->keyword##_schema != NULL)                 \
      compact_compiled_schema(compiled_schema->keyword##_schema); \
  } while(0);

#define MARK_VALUE(keyword)                       \
  do {                                            \
    if(compiled_schema->keyword##_val != Qundef)  \
      rb_gc_mark(compiled_schema->keyword##_val); \
  } while(0);

#define MARK_CHILD_SCHEMA(keyword)                             \
  do {                                                         \
    if(compiled_schema->keyword##_schema != NULL)              \
      mark_compiled_schema(compiled_schema->keyword##_schema); \
  } while(0);

#define FREE_CHILD_SCHEMA(keyword)                             \
  do {                                                         \
    if(compiled_schema->keyword##_schema != NULL)              \
      free_compiled_schema(compiled_schema->keyword##_schema); \
  } while(0);

VALUE compiled_schema_class;

/*
* Here I mark all the values even if they are immediate values like integers or other
* values which should be already marked by the schema hash object provided while creating
* the `FastJSON::Schema` instance but it's better to be safe than sorry.
*/
static void mark_compiled_schema(CompiledSchema *compiled_schema) {
  rb_gc_mark(compiled_schema->path);

  MARK_VALUE(id);
  MARK_VALUE(ref);
  MARK_VALUE(recursiveAnchor);
  MARK_VALUE(recursiveRef);

  MARK_VALUE(const);
  MARK_VALUE(enum);

  MARK_VALUE(multipleOf);
  MARK_VALUE(maximum);
  MARK_VALUE(exclusiveMaximum);
  MARK_VALUE(minimum);
  MARK_VALUE(exclusiveMinimum);

  MARK_VALUE(maxLength);
  MARK_VALUE(minLength);
  MARK_VALUE(pattern);

  MARK_VALUE(custom_format_callable);
  MARK_VALUE(custom_format_error_key);

  MARK_VALUE(items); // This will mark the `items_val` not the `items_schema`.
  MARK_VALUE(maxItems);
  MARK_VALUE(minItems);
  MARK_VALUE(uniqueItems);
  MARK_VALUE(maxContains);
  MARK_VALUE(minContains);

  MARK_VALUE(properties);
  MARK_VALUE(patternProperties);
  MARK_VALUE(maxProperties);
  MARK_VALUE(minProperties);
  MARK_VALUE(required);
  MARK_VALUE(dependencies);

  MARK_CHILD_SCHEMA(if);
  MARK_CHILD_SCHEMA(then);
  MARK_CHILD_SCHEMA(else);

  MARK_VALUE(allOf);
  MARK_VALUE(anyOf);
  MARK_VALUE(oneOf);
  MARK_CHILD_SCHEMA(not);

  MARK_CHILD_SCHEMA(items);
  MARK_CHILD_SCHEMA(contains);
  MARK_CHILD_SCHEMA(additionalItems);

  MARK_CHILD_SCHEMA(propertyNames);
  MARK_CHILD_SCHEMA(additionalProperties);

  for(size_t i = 0; i < compiled_schema->nested_schemas_count; i++)
    mark_compiled_schema(compiled_schema->nested_schemas[i]);
}

static void rb_mark_compiled_schema(void *ptr) {
  CompiledSchema *compiled_schema = (CompiledSchema *)ptr;

  if(INTERNAL_ONLY(compiled_schema)) return;

  mark_compiled_schema(compiled_schema);
}

static void free_compiled_schema(CompiledSchema *compiled_schema) {
  for(size_t i = 0; i < compiled_schema->nested_schemas_count; i++)
    free_compiled_schema(compiled_schema->nested_schemas[i]);

  if(compiled_schema->nested_schemas != NULL)
    xfree(compiled_schema->nested_schemas);

  FREE_CHILD_SCHEMA(if);
  FREE_CHILD_SCHEMA(then);
  FREE_CHILD_SCHEMA(else);

  FREE_CHILD_SCHEMA(not);

  FREE_CHILD_SCHEMA(items);
  FREE_CHILD_SCHEMA(contains);
  FREE_CHILD_SCHEMA(additionalItems);

  FREE_CHILD_SCHEMA(propertyNames);
  FREE_CHILD_SCHEMA(additionalProperties);

  xfree(compiled_schema);
}

/*
* This gets called by the Ruby GC for each `FastJSON::Schema::CompiledSchema` instance.
* As we are also wrapping the child compiled schemas(they are internal) into Ruby instances
* to pass them between Ruby methods, the pointer we receive here can refer to a child compiled
* schema which is needed by its parent.
* In that case, we shouldn't free the memory block addressed by that pointer and let the root compiled
* schema to handle freeing them.
*/
static void rb_free_compiled_schema(void *ptr) {
  CompiledSchema *compiled_schema = (CompiledSchema *)ptr;

  if(INTERNAL_ONLY(compiled_schema)) return;

  free_compiled_schema(compiled_schema);
}

/*
* Here I am re-assigning all the values including the immediate ones which can not be
* compacted as they are located in stack but there is no harm re-assigning them.
*/
static void compact_compiled_schema(CompiledSchema *compiled_schema) {
  compiled_schema->path = rb_gc_location(compiled_schema->path);

  COMPACT_VALUE(id);
  COMPACT_VALUE(ref);
  COMPACT_VALUE(recursiveAnchor);
  COMPACT_VALUE(recursiveRef);

  COMPACT_VALUE(const);
  COMPACT_VALUE(enum);

  COMPACT_VALUE(multipleOf);
  COMPACT_VALUE(maximum);
  COMPACT_VALUE(exclusiveMaximum);
  COMPACT_VALUE(minimum);
  COMPACT_VALUE(exclusiveMinimum);

  COMPACT_VALUE(maxLength);
  COMPACT_VALUE(minLength);
  COMPACT_VALUE(pattern);

  COMPACT_VALUE(custom_format_callable);
  COMPACT_VALUE(custom_format_error_key);

  COMPACT_VALUE(items); // This will compact the `items_val` not `items_schema`.
  COMPACT_VALUE(maxItems);
  COMPACT_VALUE(minItems);
  COMPACT_VALUE(uniqueItems);
  COMPACT_VALUE(maxContains);
  COMPACT_VALUE(minContains);

  COMPACT_VALUE(properties);
  COMPACT_VALUE(patternProperties);
  COMPACT_VALUE(maxProperties);
  COMPACT_VALUE(minProperties);
  COMPACT_VALUE(required);
  COMPACT_VALUE(dependencies);

  COMPACT_CHILD_SCHEMA(if);
  COMPACT_CHILD_SCHEMA(then);
  COMPACT_CHILD_SCHEMA(else);

  COMPACT_VALUE(allOf);
  COMPACT_VALUE(anyOf);
  COMPACT_VALUE(oneOf);
  COMPACT_CHILD_SCHEMA(not);

  COMPACT_CHILD_SCHEMA(items);
  COMPACT_CHILD_SCHEMA(contains);
  COMPACT_CHILD_SCHEMA(additionalItems);

  COMPACT_CHILD_SCHEMA(propertyNames);
  COMPACT_CHILD_SCHEMA(additionalProperties);

  for(size_t i = 0; i < compiled_schema->nested_schemas_count; i++)
    compact_compiled_schema(compiled_schema->nested_schemas[i]);
}

static void rb_compact_compiled_schema(void *ptr) {
  CompiledSchema *compiled_schema = (CompiledSchema *)ptr;

  if(INTERNAL_ONLY(compiled_schema)) return;

  compact_compiled_schema(compiled_schema);
}

const rb_data_type_t compiled_schema_type = {
  "Schema::CompiledSchema",
  {
    rb_mark_compiled_schema,
    rb_free_compiled_schema,
    0,
    rb_compact_compiled_schema
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

CompiledSchema *create_compiled_schema(CompiledSchema *parent, VALUE path, schema_flag_t flags) {
  CompiledSchema *compiled_schema = ALLOC(CompiledSchema);

  compiled_schema->flags = flags;

  if(parent == NULL) {
    compiled_schema->path = path;
  } else {
    compiled_schema->path = new_path(parent->path, path);
  }

  compiled_schema->id_val = Qundef;
  compiled_schema->ref_val = Qundef;
  compiled_schema->recursiveAnchor_val = Qundef;
  compiled_schema->recursiveRef_val = Qundef;

  compiled_schema->ref_schema = NULL;

  compiled_schema->const_val = Qundef;
  compiled_schema->enum_val = Qundef;

  compiled_schema->if_schema = NULL;
  compiled_schema->then_schema = NULL;
  compiled_schema->else_schema = NULL;

  compiled_schema->allOf_val = Qundef;
  compiled_schema->anyOf_val = Qundef;
  compiled_schema->oneOf_val = Qundef;
  compiled_schema->not_schema = NULL;

  compiled_schema->multipleOf_val = Qundef;
  compiled_schema->maximum_val = Qundef;
  compiled_schema->exclusiveMaximum_val = Qundef;
  compiled_schema->minimum_val = Qundef;
  compiled_schema->exclusiveMinimum_val = Qundef;

  compiled_schema->maxLength_val = Qundef;
  compiled_schema->minLength_val = Qundef;
  compiled_schema->pattern_val = Qundef;

  compiled_schema->custom_format_callable_val = Qundef;
  compiled_schema->custom_format_error_key_val = Qundef;

  compiled_schema->items_schema = NULL;
  compiled_schema->items_val = Qundef;
  compiled_schema->additionalItems_schema = NULL;
  compiled_schema->contains_schema = NULL;
  compiled_schema->maxItems_val = Qundef;
  compiled_schema->minItems_val = Qundef;
  compiled_schema->uniqueItems_val = Qundef;
  compiled_schema->maxContains_val = Qundef;
  compiled_schema->minContains_val = Qundef;

  compiled_schema->properties_val = Qundef;
  compiled_schema->patternProperties_val = Qundef;
  compiled_schema->propertyNames_schema = NULL;
  compiled_schema->additionalProperties_schema = NULL;
  compiled_schema->maxProperties_val = Qundef;
  compiled_schema->minProperties_val = Qundef;
  compiled_schema->required_val = Qundef;
  compiled_schema->dependencies_val = Qundef;

  compiled_schema->nested_schemas = NULL;
  compiled_schema->nested_schemas_count = 0;

  compiled_schema->type_flags = 0;

  return compiled_schema;
}

static unsigned int parse_type_array(VALUE arr) {
  if(RARRAY_LEN(arr) == 0)
    rb_raise(rb_eRuntimeError, "Invalid 'type' array: must contain at least one element");

  unsigned int flags = 0;
  long i;

  for(i = 0; i < RARRAY_LEN(arr); i++) {
    VALUE elem = rb_ary_entry(arr, i);

    if(!RB_TYPE_P(elem, T_STRING))
      rb_raise(rb_eRuntimeError, "Invalid 'type' array: elements must be strings");

    unsigned int flag = 0;

    if(rb_str_equal(elem, null_type_str) == Qtrue) {
      flag = TYPE_NULL;
    } else if(rb_str_equal(elem, boolean_type_str) == Qtrue) {
      flag = TYPE_BOOL;
    } else if(rb_str_equal(elem, string_type_str) == Qtrue) {
      flag = TYPE_STRING;
    } else if(rb_str_equal(elem, integer_type_str) == Qtrue) {
      flag = TYPE_INTEGER;
    } else if(rb_str_equal(elem, number_type_str) == Qtrue) {
      flag = TYPE_NUMBER;
    } else if(rb_str_equal(elem, array_type_str) == Qtrue) {
      flag = TYPE_ARRAY;
    } else if(rb_str_equal(elem, object_str) == Qtrue) {
      flag = TYPE_OBJECT;
    } else {
      rb_raise(rb_eRuntimeError, "Invalid 'type' array: unknown type %s", StringValueCStr(elem));
    }

    if(flags & flag)
      rb_raise(rb_eRuntimeError, "Invalid 'type' array: duplicate type %s", StringValueCStr(elem));

    flags |= flag;
  }

  return flags;
}

static validation_function type_validation_function(VALUE ruby_schema, CompiledSchema *compiled_schema) {
  VALUE type_val = rb_hash_aref(ruby_schema, type_str);

  if(RB_TYPE_P(type_val, T_ARRAY)) {
    compiled_schema->type_flags = parse_type_array(type_val);

    return validate_by_type_list;
  }

  if(!RB_TYPE_P(type_val, T_STRING)) return validate_by_data_type;

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

void compile(CompiledSchema *compiled_schema, VALUE ruby_schema, VALUE ref_data, VALUE custom_formats) {
  // Embed compiled schema into Ruby Hash for ref resolution
  register_schema_for_ref_resolution(compiled_schema, ref_data);

  compiled_schema->validation_function = no_op_validate;

  if(ruby_schema == Qfalse)
    compiled_schema->validation_function = false_validate;

  if(!RB_TYPE_P(ruby_schema, T_HASH))
    return;

  if(RHASH_SIZE(ruby_schema) == 0) {
    compiled_schema->validation_function = no_op_validate;

    return;
  }

  compiled_schema->validation_function = validate;

  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_1(id, T_STRING);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_1(ref, T_STRING);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_1(recursiveAnchor, T_STRING);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_1(recursiveRef, T_STRING);

  ASSIGN_ANY_VALUE_TO_COMPILED_SCHEMA(const);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_1(enum, T_ARRAY);

  ASSIGN_SCHEMA_TO_COMPILED_SCHEMA(if);
  ASSIGN_SCHEMA_TO_COMPILED_SCHEMA(then);
  ASSIGN_SCHEMA_TO_COMPILED_SCHEMA(else);

  ASSIGN_SCHEMA_COLLECTION_TO_COMPILED_SCHEMA(allOf);
  ASSIGN_SCHEMA_COLLECTION_TO_COMPILED_SCHEMA(anyOf);
  ASSIGN_SCHEMA_COLLECTION_TO_COMPILED_SCHEMA(oneOf);
  ASSIGN_SCHEMA_TO_COMPILED_SCHEMA(not);

  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_3(multipleOf, T_FIXNUM, T_BIGNUM, T_FLOAT);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_3(maximum, T_FIXNUM, T_BIGNUM, T_FLOAT);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_3(exclusiveMaximum, T_FIXNUM, T_BIGNUM, T_FLOAT);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_3(minimum, T_FIXNUM, T_BIGNUM, T_FLOAT);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_3(exclusiveMinimum, T_FIXNUM, T_BIGNUM, T_FLOAT);

  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_3(maxLength, T_FIXNUM, T_BIGNUM, T_FLOAT);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_3(minLength, T_FIXNUM, T_BIGNUM, T_FLOAT);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_1(pattern, T_STRING);

  ASSIGN_SCHEMA_TO_COMPILED_SCHEMA(items);
  ASSIGN_SCHEMA_COLLECTION_TO_COMPILED_SCHEMA(items);
  ASSIGN_SCHEMA_TO_COMPILED_SCHEMA(additionalItems);
  ASSIGN_SCHEMA_TO_COMPILED_SCHEMA(contains);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_3(maxItems, T_FIXNUM, T_BIGNUM, T_FLOAT);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_3(minItems, T_FIXNUM, T_BIGNUM, T_FLOAT);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_2(uniqueItems, T_TRUE, T_FALSE);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_3(maxContains, T_FIXNUM, T_BIGNUM, T_FLOAT);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_3(minContains, T_FIXNUM, T_BIGNUM, T_FLOAT);

  ASSIGN_SCHEMA_TO_COMPILED_SCHEMA(propertyNames);
  ASSIGN_SCHEMA_TO_COMPILED_SCHEMA(additionalProperties);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_3(maxProperties, T_FIXNUM, T_BIGNUM, T_FLOAT);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_3(minProperties, T_FIXNUM, T_BIGNUM, T_FLOAT);
  ASSIGN_TYPED_VALUE_TO_COMPILED_SCHEMA_1(required, T_ARRAY);

  compile_properties_val(compiled_schema, ruby_schema, ref_data, custom_formats);
  compile_pattern_properties_val(compiled_schema, ruby_schema, ref_data, custom_formats);
  compile_dependencies_val(compiled_schema, ruby_schema, ref_data, custom_formats);

  compile_nested_schemas(compiled_schema, ruby_schema, ref_data, custom_formats);

  VALUE format_val = rb_hash_aref(ruby_schema, format_str);
  set_format_validation_function_for_compiled_schema(compiled_schema, format_val, custom_formats);

  compiled_schema->type_validation_function = type_validation_function(ruby_schema, compiled_schema);

  if(compiled_schema->ref_val != Qundef)
    compiled_schema->validation_function = validate_ref;
}

void compile_schema(VALUE self) {
  VALUE ruby_schema = rb_ivar_get(self, rb_intern("@ruby_schema"));
  VALUE ref_data = rb_hash_new();

  /*
  * Before running the compilation logic, we have to wrap the CompiledSchema struct into a
  * Ruby object and keep it in the stack. Otherwise, we can't mark the intermediary objects,
  * created and assigned to that struct to prevent them from being freed by the GC if it runs
  * while compiling the schema.
  */
  schema_flag_t flags = ROOT_SCHEMA | EXPOSE_TO_RUBY;
  CompiledSchema *compiled_schema = create_compiled_schema(NULL, root_path_str, flags);
  VALUE compiled_schema_obj = WrapCompiledSchema(compiled_schema);

  VALUE custom_formats = rb_ivar_get(self, rb_intern("@custom_formats"));

  compile(compiled_schema, ruby_schema, ref_data, custom_formats);
  resolve_refs(compiled_schema, ref_data);

  rb_ivar_set(self, rb_intern("compiled_schema"), compiled_schema_obj);
  rb_ivar_set(self, rb_intern("compiled"), Qtrue);
}

void Init_compiled_schema(VALUE schema_class) {
  compiled_schema_class = rb_define_class_under(schema_class, "CompiledSchema", rb_cObject);

  rb_define_alloc_func(compiled_schema_class, alloc_compiled_schema);

  rb_gc_register_address(&compiled_schema_class);
}
