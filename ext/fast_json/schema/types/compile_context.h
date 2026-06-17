#ifndef FAST_JSON_SCHEMA_COMPILE_CONTEXT_H
#define FAST_JSON_SCHEMA_COMPILE_CONTEXT_H

#include <ruby.h>

typedef struct compile_context_struct {
  VALUE ref_data;
  VALUE custom_formats;
  VALUE current_base;     // absolute URI string from nearest enclosing $id, or Qnil
  VALUE current_pointer;  // JSON Pointer suffix from nearest $id ancestor (no leading '#'); "" at the base node, Qnil if no base is active
} CompileContext;

#endif
