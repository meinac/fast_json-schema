#include <ruby.h>
#include "types/context.h"

#ifndef FAST_JSON_SCHEMA_COMPILED_SCHEMA_TYPE
#define FAST_JSON_SCHEMA_COMPILED_SCHEMA_TYPE 1

typedef struct compiled_schema_struct CompiledSchema;

typedef void (*validation_function)(VALUE, CompiledSchema *, VALUE, Context *);

typedef struct compiled_schema_struct {
  VALUE path;

  VALUE id_val;
  VALUE ref_val;
  VALUE recursiveAnchor_val;
  VALUE recursiveRef_val;

  validation_function validation_function;
} CompiledSchema;

#endif
