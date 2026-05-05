#include <ruby.h>

#ifndef FAST_JSON_SCHEMA_CONTEXT_TYPE
#define FAST_JSON_SCHEMA_CONTEXT_TYPE 1

#define MAX_CONTEXT_DEPTH 1000

typedef struct context_struct {
  VALUE path[MAX_CONTEXT_DEPTH];
  int depth;
  bool short_circuit_on_error;
} Context;

VALUE create_context(Context **out_context);
void Init_context(void);

#endif

#define INCR_CONTEXT(context)                             \
  do {                                                    \
    if(++context->depth > MAX_CONTEXT_DEPTH)              \
      rb_raise(rb_eRuntimeError, "Document is too deep"); \
  } while(0);

#define DECR_CONTEXT(context) context->depth--;

#define ADD_TO_CONTEXT(context, value) context->path[context->depth] = value;
