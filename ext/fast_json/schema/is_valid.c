#include "is_valid.h"

VALUE short_circuit_tag;

struct is_valid_args_memo_S {
  VALUE schema;
  CompiledSchema *compiled_schema;
  VALUE data;
  Context *context;
};

struct ensure_args_memo_S {
  Context *context;
  bool prev_short_circuit;
};

static VALUE is_valid_body(RB_BLOCK_CALL_FUNC_ARGLIST(_tag, arg)) {
  struct is_valid_args_memo_S *args = (struct is_valid_args_memo_S *)arg;

  args->compiled_schema->validation_function(
    args->schema, args->compiled_schema, args->data, args->context
  );

  return Qtrue;
}

static VALUE is_valid_catch(VALUE arg) {
  return rb_catch_obj(short_circuit_tag, is_valid_body, arg);
}

static VALUE is_valid_ensure(VALUE arg) {
  struct ensure_args_memo_S *ensure_args = (struct ensure_args_memo_S *)arg;

  ensure_args->context->short_circuit_on_error = ensure_args->prev_short_circuit;

  return Qnil;
}

/*
 * `is_valid` runs the given compiled schema against the given data and
 * reports whether the data validates, without yielding any error to the
 * user. It is the entry point used by every combinator that needs a
 * pure pass/fail answer about a sub-schema: `anyOf`, `oneOf`, `allOf`,
 * `not`, `if`, and the array `contains` keyword.
 *
 * Why short-circuiting matters
 * ----------------------------
 * Validation is normally driven by side effects: when a constraint is
 * violated, `yield_error` builds an `Error` and yields it to the user
 * block. Combinators don't want that. As soon as `anyOf` (for example)
 * sees the *first* failure inside a branch, it knows that branch is
 * invalid and wants to abandon it immediately, no matter how deeply
 * nested the failing constraint is. Letting validation run to completion
 * to discover the failure would also leak the branch's internal errors
 * to the user, which is wrong.
 *
 * How the short-circuit works
 * ---------------------------
 * Two pieces of state cooperate:
 *
 *   1. `context->short_circuit_on_error` (a `bool`) tells `yield_error`
 *      whether it should yield (the default, top-level behavior) or
 *      short-circuit (when an `is_valid` frame is active). The flag is
 *      set to `true` here on entry and restored to its prior value on
 *      exit, regardless of how the body unwinds.
 *
 *   2. `short_circuit_tag` (a unique frozen `Object.new` allocated in
 *      `Init_validate`) is the tag we use with Ruby's `throw`/`catch`
 *      mechanism. When `yield_error` is reached with the flag set, it
 *      calls `rb_throw_obj(short_circuit_tag, Qfalse)`, which Ruby's
 *      VM unwinds up to the matching `rb_catch_obj` registered below.
 *
 * Why throw/catch instead of setjmp/longjmp
 * -----------------------------------------
 * An earlier implementation used `RUBY_SETJMP`/`RUBY_LONGJMP` to escape
 * out of nested validation. That worked on Ruby 2.x, but in Ruby 3.x it
 * corrupts VM state when the jump crosses Ruby callbacks like
 * `rb_funcall`, `rb_reg_match`, `rb_yield`, or `rb_hash_foreach`: those
 * functions register frame and iteration bookkeeping that `longjmp`
 * skips over, leaving the VM and GC in an inconsistent state and
 * eventually segfaulting. `rb_throw_obj` performs the same logical
 * non-local exit but uses the VM's own unwind machinery, which closes
 * frames cleanly, releases hash iteration state, and runs intervening
 * `ensure` blocks.
 *
 * Why we still need `rb_ensure`
 * -----------------------------
 * `rb_catch_obj` only intercepts throws matching its tag. A real Ruby
 * exception -- e.g. the `RuntimeError` raised by `INCR_CONTEXT` when
 * the document exceeds `MAX_CONTEXT_DEPTH`, or a user `raise` from
 * inside the validate block -- is not a throw; it propagates straight
 * through. We wrap the catch in `rb_ensure` so that even when an
 * exception escapes, the ensure callback restores
 * `short_circuit_on_error` to its prior value before unwinding
 * continues. Without that, an active `is_valid` frame interrupted by
 * a real exception would leave the flag stuck at `true`, silently
 * breaking subsequent validations that share the same context.
 *
 * Nesting
 * -------
 * Combinators routinely nest (e.g. `anyOf` containing `oneOf`
 * containing `allOf`). Saving and restoring the flag around each
 * `is_valid` call, combined with `rb_catch_obj`'s LIFO semantics
 * (each throw is caught by the nearest matching catch), gives correct
 * behavior at any depth: an inner short-circuit unwinds only to the
 * inner catch, leaving the outer combinator free to continue
 * iterating its remaining branches with the flag still set.
 */
bool is_valid(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  struct is_valid_args_memo_S is_valid_args = { schema, compiled_schema, data, context };
  struct ensure_args_memo_S ensure_args = { context, context->short_circuit_on_error };

  context->short_circuit_on_error = true;

  VALUE result = rb_ensure(is_valid_catch, (VALUE)&is_valid_args, is_valid_ensure, (VALUE)&ensure_args);

  return result == Qtrue;
}

void Init_is_valid(void) {
  short_circuit_tag = rb_obj_freeze(rb_class_new_instance(0, NULL, rb_cObject));

  rb_gc_register_address(&short_circuit_tag);
}