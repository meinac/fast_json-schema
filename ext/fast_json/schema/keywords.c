#include "keywords.h"

VALUE root_path_str;

VALUE id_str,
      ref_str,
      recursiveAnchor_str,
      recursiveRef_str;

VALUE type_str,
      const_str,
      enum_str;

VALUE if_str,
      then_str,
      else_str,
      allOf_str,
      anyOf_str,
      oneOf_str,
      not_str;

// Integer and Number keywords
VALUE multipleOf_str,
      maximum_str,
      exclusiveMaximum_str,
      minimum_str,
      exclusiveMinimum_str;

// String keywords
VALUE maxLength_str,
      minLength_str,
      pattern_str,
      format_str;

// Array keywords
VALUE items_str,
      additionalItems_str,
      unevaluatedItems_str,
      contains_str,
      maxItems_str,
      minItems_str,
      uniqueItems_str,
      maxContains_str,
      minContains_str;

// Object keywords
VALUE properties_str,
      patternProperties_str,
      additionalProperties_str,
      unevaluatedProperties_str,
      propertyNames_str,
      maxProperties_str,
      minProperties_str,
      required_str,
      dependentRequired_str,
      dependencies_str;

VALUE object_str;

VALUE known_keywords_hash;

bool is_known_keyword(VALUE key) {
  if(!RB_TYPE_P(key, T_STRING)) return false;

  return rb_hash_lookup(known_keywords_hash, key) != Qnil;
}

void Init_keywords() {
  root_path_str = rb_str_new_literal("#");

  id_str = rb_str_new_literal("$id");
  ref_str = rb_str_new_literal("$ref");
  recursiveAnchor_str = rb_str_new_literal("$recursiveAnchor");
  recursiveRef_str = rb_str_new_literal("$recursiveRef");

  type_str = rb_str_new_literal("type");
  const_str = rb_str_new_literal("const");
  enum_str = rb_str_new_literal("enum");

  if_str = rb_str_new_literal("if");
  then_str = rb_str_new_literal("then");
  else_str = rb_str_new_literal("else");
  allOf_str = rb_str_new_literal("allOf");
  anyOf_str = rb_str_new_literal("anyOf");
  oneOf_str = rb_str_new_literal("oneOf");
  not_str = rb_str_new_literal("not");

  multipleOf_str = rb_str_new_literal("multipleOf");
  maximum_str = rb_str_new_literal("maximum");
  exclusiveMaximum_str = rb_str_new_literal("exclusiveMaximum");
  minimum_str = rb_str_new_literal("minimum");
  exclusiveMinimum_str = rb_str_new_literal("exclusiveMinimum");

  maxLength_str = rb_str_new_literal("maxLength");
  minLength_str = rb_str_new_literal("minLength");
  pattern_str = rb_str_new_literal("pattern");
  format_str = rb_str_new_literal("format");

  items_str = rb_str_new_literal("items");
  additionalItems_str = rb_str_new_literal("additionalItems");
  unevaluatedItems_str = rb_str_new_literal("unevaluatedItems");
  contains_str = rb_str_new_literal("contains");
  maxItems_str = rb_str_new_literal("maxItems");
  minItems_str = rb_str_new_literal("minItems");
  uniqueItems_str = rb_str_new_literal("uniqueItems");
  maxContains_str = rb_str_new_literal("maxContains");
  minContains_str = rb_str_new_literal("minContains");

  properties_str = rb_str_new_literal("properties");
  patternProperties_str = rb_str_new_literal("patternProperties");
  additionalProperties_str = rb_str_new_literal("additionalProperties");
  unevaluatedProperties_str = rb_str_new_literal("unevaluatedProperties");
  propertyNames_str = rb_str_new_literal("propertyNames");
  maxProperties_str = rb_str_new_literal("maxProperties");
  minProperties_str = rb_str_new_literal("minProperties");
  required_str = rb_str_new_literal("required");
  dependentRequired_str = rb_str_new_literal("dependentRequired");
  dependencies_str = rb_str_new_literal("dependencies");

  object_str = rb_str_new_literal("object");

  known_keywords_hash = rb_hash_new();
  rb_hash_aset(known_keywords_hash, id_str, Qtrue);
  rb_hash_aset(known_keywords_hash, ref_str, Qtrue);
  rb_hash_aset(known_keywords_hash, recursiveAnchor_str, Qtrue);
  rb_hash_aset(known_keywords_hash, recursiveRef_str, Qtrue);
  rb_hash_aset(known_keywords_hash, type_str, Qtrue);
  rb_hash_aset(known_keywords_hash, const_str, Qtrue);
  rb_hash_aset(known_keywords_hash, enum_str, Qtrue);
  rb_hash_aset(known_keywords_hash, if_str, Qtrue);
  rb_hash_aset(known_keywords_hash, then_str, Qtrue);
  rb_hash_aset(known_keywords_hash, else_str, Qtrue);
  rb_hash_aset(known_keywords_hash, allOf_str, Qtrue);
  rb_hash_aset(known_keywords_hash, anyOf_str, Qtrue);
  rb_hash_aset(known_keywords_hash, oneOf_str, Qtrue);
  rb_hash_aset(known_keywords_hash, not_str, Qtrue);
  rb_hash_aset(known_keywords_hash, multipleOf_str, Qtrue);
  rb_hash_aset(known_keywords_hash, maximum_str, Qtrue);
  rb_hash_aset(known_keywords_hash, exclusiveMaximum_str, Qtrue);
  rb_hash_aset(known_keywords_hash, minimum_str, Qtrue);
  rb_hash_aset(known_keywords_hash, exclusiveMinimum_str, Qtrue);
  rb_hash_aset(known_keywords_hash, maxLength_str, Qtrue);
  rb_hash_aset(known_keywords_hash, minLength_str, Qtrue);
  rb_hash_aset(known_keywords_hash, pattern_str, Qtrue);
  rb_hash_aset(known_keywords_hash, format_str, Qtrue);
  rb_hash_aset(known_keywords_hash, items_str, Qtrue);
  rb_hash_aset(known_keywords_hash, additionalItems_str, Qtrue);
  rb_hash_aset(known_keywords_hash, unevaluatedItems_str, Qtrue);
  rb_hash_aset(known_keywords_hash, contains_str, Qtrue);
  rb_hash_aset(known_keywords_hash, maxItems_str, Qtrue);
  rb_hash_aset(known_keywords_hash, minItems_str, Qtrue);
  rb_hash_aset(known_keywords_hash, uniqueItems_str, Qtrue);
  rb_hash_aset(known_keywords_hash, maxContains_str, Qtrue);
  rb_hash_aset(known_keywords_hash, minContains_str, Qtrue);
  rb_hash_aset(known_keywords_hash, properties_str, Qtrue);
  rb_hash_aset(known_keywords_hash, patternProperties_str, Qtrue);
  rb_hash_aset(known_keywords_hash, additionalProperties_str, Qtrue);
  rb_hash_aset(known_keywords_hash, unevaluatedProperties_str, Qtrue);
  rb_hash_aset(known_keywords_hash, propertyNames_str, Qtrue);
  rb_hash_aset(known_keywords_hash, maxProperties_str, Qtrue);
  rb_hash_aset(known_keywords_hash, minProperties_str, Qtrue);
  rb_hash_aset(known_keywords_hash, required_str, Qtrue);
  rb_hash_aset(known_keywords_hash, dependentRequired_str, Qtrue);
  rb_hash_aset(known_keywords_hash, dependencies_str, Qtrue);

  rb_gc_register_address(&known_keywords_hash);

  rb_gc_register_address(&root_path_str);

  rb_gc_register_address(&id_str);
  rb_gc_register_address(&ref_str);
  rb_gc_register_address(&recursiveAnchor_str);
  rb_gc_register_address(&recursiveRef_str);

  rb_gc_register_address(&type_str);
  rb_gc_register_address(&const_str);
  rb_gc_register_address(&enum_str);

  rb_gc_register_address(&if_str);
  rb_gc_register_address(&then_str);
  rb_gc_register_address(&else_str);
  rb_gc_register_address(&allOf_str);
  rb_gc_register_address(&anyOf_str);
  rb_gc_register_address(&oneOf_str);
  rb_gc_register_address(&not_str);

  rb_gc_register_address(&multipleOf_str);
  rb_gc_register_address(&maximum_str);
  rb_gc_register_address(&exclusiveMaximum_str);
  rb_gc_register_address(&minimum_str);
  rb_gc_register_address(&exclusiveMinimum_str);

  rb_gc_register_address(&maxLength_str);
  rb_gc_register_address(&minLength_str);
  rb_gc_register_address(&pattern_str);
  rb_gc_register_address(&format_str);

  rb_gc_register_address(&items_str);
  rb_gc_register_address(&additionalItems_str);
  rb_gc_register_address(&unevaluatedItems_str);
  rb_gc_register_address(&contains_str);
  rb_gc_register_address(&maxItems_str);
  rb_gc_register_address(&minItems_str);
  rb_gc_register_address(&uniqueItems_str);
  rb_gc_register_address(&maxContains_str);
  rb_gc_register_address(&minContains_str);

  rb_gc_register_address(&properties_str);
  rb_gc_register_address(&patternProperties_str);
  rb_gc_register_address(&additionalProperties_str);
  rb_gc_register_address(&unevaluatedProperties_str);
  rb_gc_register_address(&propertyNames_str);
  rb_gc_register_address(&maxProperties_str);
  rb_gc_register_address(&minProperties_str);
  rb_gc_register_address(&required_str);
  rb_gc_register_address(&dependentRequired_str);
  rb_gc_register_address(&dependencies_str);

  rb_gc_register_address(&object_str);
}
