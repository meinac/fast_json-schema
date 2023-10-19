#include <ruby.h>
#include <setjmp.h>

#ifndef FAST_JSON_SCHEMA_CONTEXT_TYPE
#define FAST_JSON_SCHEMA_CONTEXT_TYPE 1

#define MAX_CONTEXT_DEPTH 1000

typedef struct validation_env_struct {
  bool jump;
  jmp_buf buf;
} ValidationEnv;

typedef struct context_struct {
  VALUE path[MAX_CONTEXT_DEPTH];
  int depth;
  ValidationEnv env;
} Context;

#endif

#define INCR_CONTEXT(context)                             \
  do {                                                    \
    if(++context->depth > MAX_CONTEXT_DEPTH)              \
      rb_raise(rb_eRuntimeError, "Document is too deep"); \
  } while(0);

#define DECR_CONTEXT(context) context->depth--;

#define ADD_TO_CONTEXT(context, value) context->path[context->depth] = value;

#define SET_JUMP(env, old_env, result)               \
  do {                                               \
    if(env.jump) {                                   \
      memcpy(&old_env, &env, sizeof(ValidationEnv)); \
    } else {                                         \
      old_env.jump = false;                          \
    }                                                \
                                                     \
    env.jump = true;                                 \
    result = RUBY_SETJMP(env.buf);                   \
  } while(0);

#define RESET_JUMP(env, old_env)                     \
  do {                                               \
    if(old_env.jump) {                               \
      memcpy(&env, &old_env, sizeof(ValidationEnv)); \
    } else {                                         \
      env.jump = false;                              \
    }                                                \
  } while(0);

#define JUMP_IF_SET(env)        \
  do {                          \
    if(env.jump)                \
      RUBY_LONGJMP(env.buf, 1); \
  } while(0);
