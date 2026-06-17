#include "uri_scope.h"
#include "uri_resolver.h"

static void resolve_ref_against_parent_base(CompiledSchema *compiled_schema, CompileContext *ctx) {
  if(compiled_schema->ref_val == Qundef) return;

  if(uri_resolver_is_absolute(compiled_schema->ref_val)) {
    compiled_schema->resolvedRef_val = compiled_schema->ref_val;

    return;
  }

  VALUE joined = uri_resolver_join(ctx->current_base, compiled_schema->ref_val);

  if(!NIL_P(joined)) compiled_schema->resolvedRef_val = joined;
}

static void snapshot_scope_pointer(CompiledSchema *compiled_schema, CompileContext *ctx) {
  if(ctx->current_pointer == Qnil || ctx->current_pointer == Qundef) return;

  compiled_schema->scopePointer_val = ctx->current_pointer;
}

static VALUE resolve_id_against_parent_base(CompiledSchema *compiled_schema, CompileContext *ctx) {
  if(uri_resolver_is_absolute(compiled_schema->id_val))
    return compiled_schema->id_val;

  VALUE joined = uri_resolver_join(ctx->current_base, compiled_schema->id_val);

  return NIL_P(joined) ? compiled_schema->id_val : joined;
}

CompileContext resolve_uri_scope(CompiledSchema *compiled_schema, CompileContext *ctx) {
  /*
  * $ref resolves against the parent base BEFORE any sibling $id can change it.
  * This encodes the Draft 7 rule that $ref ignores sibling keywords.
  */
  resolve_ref_against_parent_base(compiled_schema, ctx);

  // Snapshot the JSON-Pointer suffix within the active base before any reset for $id.
  snapshot_scope_pointer(compiled_schema, ctx);

  // Build the child context. If this schema has its own $id, resolve it and use it as the new base.
  CompileContext child_ctx = *ctx;

  if(compiled_schema->id_val != Qundef) {
    VALUE resolved_id = resolve_id_against_parent_base(compiled_schema, ctx);

    compiled_schema->resolvedId_val = resolved_id;
    child_ctx.current_base = resolved_id;
    child_ctx.current_pointer = rb_str_new_cstr("");
  }

  return child_ctx;
}
