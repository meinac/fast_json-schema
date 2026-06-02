#ifndef FAST_JSON_SCHEMA_COMPILE_CONTEXT_H
#define FAST_JSON_SCHEMA_COMPILE_CONTEXT_H

#include <ruby.h>

typedef struct compile_context_S {
  VALUE ref_data;
  VALUE custom_formats;
} CompileContext;

#endif
