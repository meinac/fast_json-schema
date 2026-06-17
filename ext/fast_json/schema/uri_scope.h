#ifndef FAST_JSON_SCHEMA_URI_SCOPE_H
#define FAST_JSON_SCHEMA_URI_SCOPE_H

#include <ruby.h>
#include "types/compiled_schema.h"
#include "types/compile_context.h"

/*
* Why this function exists?
* -------------------------
* In JSON Schema, every subschema has a "base URI" -- the absolute URI of
* the nearest enclosing $id. Relative $refs and relative $ids are resolved
* against that base, the same way relative URLs work in an HTML page.
*
* Example:
*
*   {
*     "$id": "http://example.com/root/",       // base for everything below
*     "allOf": [ { "$ref": "integer" } ],      // -> http://example.com/root/integer
*     "definitions": {
*       "S": {
*         "$id": "http://example.com/root/integer",
*         "type": "integer"
*       }
*     }
*   }
*
* Without tracking the base, the $ref "integer" is just the string
* "integer" and has nothing to match. With the base, we know it really
* means "http://example.com/root/integer", which is the $id of S.
*
* A nested $id changes the base for everything inside it:
*
*   {
*     "$id": "http://example.com/a.json",
*     "properties": {
*       "foo": {
*         "$id": "b.json",                    // new base: http://example.com/b.json
*         "allOf": [ { "$ref": "#/x" } ]      // resolves inside b.json, not a.json
*       }
*     }
*   }
*
* One more rule worth calling out: $ref ignores its siblings, including a
* sibling $id. So in:
*
*   { "$id": "sub.json", "$ref": "other" }
*
* the $ref "other" is resolved against the *parent's* base, not against
* "sub.json". That's why we resolve $ref before we let $id rebind the base.
*
* What this file does
* -------------------
* For each compiled schema, resolve_uri_scope():
*
*   1. Resolves $ref against the parent's base (before any sibling $id
*      kicks in) and stores the result in resolvedRef_val.
*
*        parent base: "http://example.com/root/"
*        $ref:        "integer"
*        resolved:    "http://example.com/root/integer"
*
*   2. Remembers where we are inside the current base as a JSON Pointer
*      (scopePointer_val). This lets the ref resolver register the schema
*      under "<base>#<pointer>" so refs like
*      "http://example.com/a.json#/definitions/x" can find it.
*
*   3. If this schema has its own $id, resolves it against the parent base
*      and returns a child context whose base is the new $id and whose
*      pointer resets to "". Everything below now resolves against the
*      nearer base.
*
* Keeping all of this in one file means compile() stays simple: it asks
* resolve_uri_scope() for the right child context, registers the schema,
* and recurses. The URI rules (which are the fiddly part of Draft 7)
* live here behind a small interface.
*/
CompileContext resolve_uri_scope(CompiledSchema *compiled_schema, CompileContext *ctx);

#endif
