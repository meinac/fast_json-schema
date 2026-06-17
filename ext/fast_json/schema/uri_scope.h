#ifndef FAST_JSON_SCHEMA_URI_SCOPE_H
#define FAST_JSON_SCHEMA_URI_SCOPE_H

#include <ruby.h>
#include "types/compiled_schema.h"
#include "types/compile_context.h"

/*
* Resolves the URI-related state for a single compiled schema:
*
*   - Resolves $ref against the parent base (ctx->current_base) and stores it
*     in compiled_schema->resolvedRef_val.
*   - Snapshots ctx->current_pointer into compiled_schema->scopePointer_val.
*   - If the schema carries $id, resolves it against the parent base and stores
*     it in compiled_schema->resolvedId_val.
*
* Returns a child compile context to use for descendants:
*   - If the schema has its own $id, current_base becomes the resolved $id and
*     current_pointer resets to "".
*   - Otherwise the child context is identical to the input ctx.
*/
CompileContext resolve_uri_scope(CompiledSchema *compiled_schema, CompileContext *ctx);

#endif
