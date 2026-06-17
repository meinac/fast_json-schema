#include "ref_resolver.h"
#include "value_pointer_caster.h"
#include "ref_decoder.h"

static int resolve_refs_in_hash(VALUE key, VALUE value, VALUE data) {
  CompiledSchema *child;
  GetCompiledSchema(value, child);

  resolve_refs(child, data);

  return ST_CONTINUE;
}

static void resolve_refs_for_nested_schemas(CompiledSchema **children, size_t count, VALUE ref_data) {
  for(size_t i = 0; i < count; i++)
    resolve_refs(children[i], ref_data);
}

static void resolve_refs_in_array(VALUE array, VALUE ref_data) {
  long i;

  for(i = 0; i < RARRAY_LEN(array); i++) {
    VALUE child_obj = rb_ary_entry(array, i);

    CompiledSchema *child;
    GetCompiledSchema(child_obj, child);

    resolve_refs(child, ref_data);
  }
}

void resolve_refs(CompiledSchema *compiled_schema, VALUE ref_data) {
  if(compiled_schema == NULL) return;

  if(compiled_schema->ref_val != Qundef) {
    VALUE lookup_key = compiled_schema->resolvedRef_val != Qundef ? compiled_schema->resolvedRef_val : compiled_schema->ref_val;
    VALUE decoded_ref = decode_ref(lookup_key);
    VALUE ref_schema_ptr = rb_hash_lookup(ref_data, decoded_ref);

    /*
    * Fall back to the literal $ref if the resolved form misses (helps with anchors
    * that were registered under the literal key in Stage 1).
    */
    if(NIL_P(ref_schema_ptr) && lookup_key != compiled_schema->ref_val) {
      VALUE fallback = decode_ref(compiled_schema->ref_val);
      ref_schema_ptr = rb_hash_lookup(ref_data, fallback);
      RB_GC_GUARD(fallback);
    }

    if(NIL_P(ref_schema_ptr)) {
      // Raise an error if the $ref is not resolved.
      if(compiled_schema->resolvedRef_val != Qundef &&
         rb_str_equal(compiled_schema->resolvedRef_val, compiled_schema->ref_val) == Qfalse) {
        rb_raise(rb_eRuntimeError, "Unresolved $ref: %s (resolved to %s)",
          StringValueCStr(compiled_schema->ref_val),
          StringValueCStr(compiled_schema->resolvedRef_val));
      } else {
        rb_raise(rb_eRuntimeError, "Unresolved $ref: %s", StringValueCStr(compiled_schema->ref_val));
      }
    }

    compiled_schema->ref_schema = (CompiledSchema *)NUM2PTR(ref_schema_ptr);

    RB_GC_GUARD(decoded_ref);
  }

  resolve_refs(compiled_schema->if_schema, ref_data);
  resolve_refs(compiled_schema->then_schema, ref_data);
  resolve_refs(compiled_schema->else_schema, ref_data);
  resolve_refs(compiled_schema->not_schema, ref_data);
  resolve_refs(compiled_schema->items_schema, ref_data);
  resolve_refs(compiled_schema->additionalItems_schema, ref_data);
  resolve_refs(compiled_schema->contains_schema, ref_data);
  resolve_refs(compiled_schema->propertyNames_schema, ref_data);
  resolve_refs(compiled_schema->additionalProperties_schema, ref_data);

  if(compiled_schema->allOf_val != Qundef)
    resolve_refs_in_array(compiled_schema->allOf_val, ref_data);

  if(compiled_schema->anyOf_val != Qundef)
    resolve_refs_in_array(compiled_schema->anyOf_val, ref_data);

  if(compiled_schema->oneOf_val != Qundef)
    resolve_refs_in_array(compiled_schema->oneOf_val, ref_data);

  if(RB_TYPE_P(compiled_schema->items_val, T_ARRAY))
    resolve_refs_in_array(compiled_schema->items_val, ref_data);

  if(compiled_schema->properties_val != Qundef)
    rb_hash_foreach(compiled_schema->properties_val, resolve_refs_in_hash, ref_data);

  if(compiled_schema->patternProperties_val != Qundef)
    rb_hash_foreach(compiled_schema->patternProperties_val, resolve_refs_in_hash, ref_data);

  if(compiled_schema->dependencies_val != Qundef)
    rb_hash_foreach(compiled_schema->dependencies_val, resolve_refs_in_hash, ref_data);

  if(compiled_schema->nested_schemas != NULL)
    resolve_refs_for_nested_schemas(compiled_schema->nested_schemas, compiled_schema->nested_schemas_count, ref_data);
}

void register_path_for_ref_resolution(CompiledSchema *compiled_schema, VALUE ref_data) {
  rb_hash_aset(ref_data, compiled_schema->path, PTR2NUM(compiled_schema));
}

void register_id_for_ref_resolution(CompiledSchema *compiled_schema, VALUE ref_data, VALUE enclosing_base) {
  // Index by the literal `$id`
  if(compiled_schema->id_val != Qundef)
    rb_hash_aset(ref_data, compiled_schema->id_val, PTR2NUM(compiled_schema));

  // Index by the resolved absolute `$id` (when the join produced something different).
  if(compiled_schema->resolvedId_val != Qundef &&
     compiled_schema->resolvedId_val != compiled_schema->id_val) {
    rb_hash_aset(ref_data, compiled_schema->resolvedId_val, PTR2NUM(compiled_schema));
  }

  /*
  * Index by "<enclosing_base>#<scopePointer_val>" so $refs that are JSON-Pointers
  * inside a non-root base resolve through this schema's location.
  */
  if(!NIL_P(enclosing_base) && compiled_schema->scopePointer_val != Qundef) {
    long ptr_len = RSTRING_LEN(compiled_schema->scopePointer_val);

    if(ptr_len > 0) {
      VALUE combined = rb_str_dup(enclosing_base);
      rb_str_cat_cstr(combined, "#");
      rb_str_buf_append(combined, compiled_schema->scopePointer_val);

      rb_hash_aset(ref_data, combined, PTR2NUM(compiled_schema));
    }
  }
}