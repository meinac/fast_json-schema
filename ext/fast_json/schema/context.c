#include "context.h"

#define MAX_CONTEXT_DEPTH 100

VALUE context_class;

static void mark_context(void *ptr) {
  int i;
  Context *context = (Context *)ptr;

  for(i = 0; i < context->depth; i++)
    rb_gc_mark(context->path[i]);
}

static void free_context(void *ptr) {
  Context *context = (Context *)ptr;

  /*
  * If `used_in` is not `0` then it means that there are still
  * some ruby objects holding reference to this pointer.
  */
  if(--context->used_in != 0) return;

  xfree(context->path);
  xfree(context);
}

static void compact_context(void *ptr) {
  int i;
  Context *context = (Context *)ptr;

  for(i = 0; i < context->depth; i++)
    context->path[i] = rb_gc_location(context->path[i]);
}

const rb_data_type_t context_type = {
  "Schema::Context",
  {
    mark_context,
    free_context,
    0,
    compact_context
  },
  0,
  0,
  0,
};

static VALUE alloc_context(VALUE klass) {
  Context *context;
  VALUE object = TypedData_Make_Struct(klass, Context, &context_type, context);

  context->used_in = 1;

  return object;
}

static Context *create_context(VALUE path) {
  Context *context = ALLOC(Context);

  context->path = ALLOC_N(VALUE, MAX_CONTEXT_DEPTH);
  context->depth = 1;
  context->path[0] = path;

  return context;
}

VALUE create_context_object(VALUE path) {
  Context *context = create_context(path);
  context->used_in = 1;

  return TypedData_Wrap_Struct(context_class, &context_type, context);
}

void Init_context(VALUE schema_class) {
  context_class = rb_define_class_under(schema_class, "Context", rb_cObject);

  rb_define_alloc_func(context_class, alloc_context);
}
