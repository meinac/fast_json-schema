#include <ruby.h>
#include "types/context.h"

#define NO_FLAG 0
#define ROOT_SCHEMA (1 << 0)
#define EXPOSE_TO_RUBY (1 << 1)

#define IS_CHILD(compiled_schema) ((compiled_schema->flags & ROOT_SCHEMA) == 0)
#define INTERNAL_ONLY(compiled_schema) ((compiled_schema->flags & EXPOSE_TO_RUBY) == 0)

#ifndef FAST_JSON_SCHEMA_COMPILED_SCHEMA_TYPE
#define FAST_JSON_SCHEMA_COMPILED_SCHEMA_TYPE 1

typedef struct compiled_schema_struct CompiledSchema;

typedef void (*validation_function)(VALUE, CompiledSchema *, VALUE, Context *);

typedef unsigned char schema_flag_t;

/*
* TODO:
* Currently all the validation keywords are stored flat in this struct
* which is a waste of memory. I'm planning to use union for sustaining
* the data locality and reducing the amount of memory wasted.
*/
typedef struct compiled_schema_struct {
  schema_flag_t flags;
  VALUE path;

  VALUE id_val;
  VALUE ref_val;
  VALUE recursiveAnchor_val;
  VALUE recursiveRef_val;

  validation_function validation_function;
  validation_function type_validation_function;

  VALUE const_val;
  VALUE enum_val;

  CompiledSchema *if_schema;
  CompiledSchema *then_schema;
  CompiledSchema *else_schema;

  VALUE allOf_val;
  VALUE anyOf_val;
  VALUE oneOf_val;
  CompiledSchema *not_schema;

  VALUE multipleOf_val;
  VALUE maximum_val;
  VALUE exclusiveMaximum_val;
  VALUE minimum_val;
  VALUE exclusiveMinimum_val;

  VALUE maxLength_val;
  VALUE minLength_val;
  VALUE pattern_val;

  /*
  * `items` can be either a JSON schema or an array of valid JSON schemas.
  *
  *   - `items_schema` is set if it's a JSON schema
  *   - `items_val` is set if it's an array of JSON schemas
  */
  CompiledSchema *items_schema;
  VALUE items_val;
  CompiledSchema *additionalItems_schema;
  CompiledSchema *contains_schema;
  VALUE maxItems_val;
  VALUE minItems_val;
  VALUE uniqueItems_val;
  VALUE maxContains_val;
  VALUE minContains_val;

  VALUE properties_val;
  VALUE patternProperties_val;
  CompiledSchema *propertyNames_schema;
  CompiledSchema *additionalProperties_schema;
  VALUE maxProperties_val;
  VALUE minProperties_val;
  VALUE required_val;
  VALUE dependentRequired_val;
} CompiledSchema;

#endif
