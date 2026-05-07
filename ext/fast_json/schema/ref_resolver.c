#include "ref_resolver.h"
#include "value_pointer_caster.h"

static int resolve_refs_in_hash(VALUE key, VALUE value, VALUE data) {
  CompiledSchema *child;
  GetCompiledSchema(value, child);

  resolve_refs(child, data);

  return ST_CONTINUE;
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
    VALUE ref_schema_ptr = rb_hash_lookup(ref_data, compiled_schema->ref_val);

    if(NIL_P(ref_schema_ptr))
      rb_raise(rb_eRuntimeError, "Unresolved $ref: %s", StringValueCStr(compiled_schema->ref_val));

    compiled_schema->ref_schema = (CompiledSchema *)NUM2PTR(ref_schema_ptr);

    return;
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
}

void register_schema_for_ref_resolution(CompiledSchema *compiled_schema, VALUE ref_data) {
  // Embed compiled schema into Ruby Hash
  rb_hash_aset(ref_data, compiled_schema->path, PTR2NUM(compiled_schema));

  // Embed compiled schema into Ruby Hash again if it has an $id
  if(compiled_schema->id_val != Qundef)
    rb_hash_aset(ref_data, compiled_schema->id_val, PTR2NUM(compiled_schema));
}