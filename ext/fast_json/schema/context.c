#include "types/context.h"
#include "keywords.h"

static VALUE context_class;

static void mark_context(void *ptr) {
  Context *context = (Context *)ptr;
  int i;

  for(i = 0; i <= context->depth; i++) {
    rb_gc_mark_movable(context->path[i]);
  }
}

static void compact_context(void *ptr) {
  Context *context = (Context *)ptr;
  int i;

  for(i = 0; i <= context->depth; i++) {
    context->path[i] = rb_gc_location(context->path[i]);
  }
}

static void free_context(void *ptr) {
  xfree(ptr);
}

static size_t context_size(const void *ptr) {
  return sizeof(Context);
}

static const rb_data_type_t context_type = {
  "Schema::Context",
  {
    mark_context,
    free_context,
    context_size,
    compact_context
  },
  0,
  0,
  RUBY_TYPED_FREE_IMMEDIATELY,
};

static VALUE alloc_context(VALUE klass) {
  Context *context;
  VALUE object = TypedData_Make_Struct(klass, Context, &context_type, context);
  int i;

  for(i = 0; i < MAX_CONTEXT_DEPTH; i++) {
    context->path[i] = Qnil;
  }

  context->depth = 0;
  context->path[0] = root_path_str;
  context->env = (ValidationEnv){ false };

  return object;
}

VALUE create_context(Context **out_context) {
  VALUE wrapper = rb_class_new_instance(0, NULL, context_class);
  Context *context;

  TypedData_Get_Struct(wrapper, Context, &context_type, context);

  *out_context = context;

  return wrapper;
}

void Init_context(void) {
  context_class = rb_class_new(rb_cObject);

  rb_define_alloc_func(context_class, alloc_context);

  rb_gc_register_address(&context_class);
}
