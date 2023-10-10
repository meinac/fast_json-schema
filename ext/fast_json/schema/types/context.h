#include <ruby.h>

#ifndef FAST_JSON_SCHEMA_CONTEXT_TYPE
#define FAST_JSON_SCHEMA_CONTEXT_TYPE 1

#define MAX_CONTEXT_DEPTH 1000

typedef struct context_struct {
  VALUE path[MAX_CONTEXT_DEPTH];
  int depth;
} Context;

#endif
