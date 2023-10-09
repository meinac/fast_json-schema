#include <ruby.h>

#ifndef FAST_JSON_SCHEMA_CONTEXT_TYPE
#define FAST_JSON_SCHEMA_CONTEXT_TYPE 1

typedef struct context_struct {
  int used_in; // This stores the number of Ruby objects holding reference to the pointer as we will share the same pointer between multiple ruby objects.
  VALUE *path;
  int depth;
} Context;

#endif
