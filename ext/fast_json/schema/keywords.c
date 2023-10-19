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
      maxProperties_str,
      minProperties_str,
      required_str,
      dependentRequired_str;

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
  maxProperties_str = rb_str_new_literal("maxProperties");
  minProperties_str = rb_str_new_literal("minProperties");
  required_str = rb_str_new_literal("required");
  dependentRequired_str = rb_str_new_literal("dependentRequired");

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
  rb_gc_register_address(&contains_str);
  rb_gc_register_address(&maxItems_str);
  rb_gc_register_address(&minItems_str);
  rb_gc_register_address(&uniqueItems_str);
  rb_gc_register_address(&maxContains_str);
  rb_gc_register_address(&minContains_str);

  rb_gc_register_address(&properties_str);
  rb_gc_register_address(&maxProperties_str);
  rb_gc_register_address(&minProperties_str);
  rb_gc_register_address(&required_str);
  rb_gc_register_address(&dependentRequired_str);
}
