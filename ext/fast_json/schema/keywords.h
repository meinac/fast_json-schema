#include <ruby.h>

extern VALUE root_path_str;

extern VALUE id_str,
             ref_str,
             recursiveAnchor_str,
             recursiveRef_str;

extern VALUE type_str,
             const_str,
             enum_str;

extern VALUE if_str,
             then_str,
             else_str,
             allOf_str,
             anyOf_str,
             oneOf_str,
             not_str;

extern VALUE multipleOf_str,
             maximum_str,
             exclusiveMaximum_str,
             minimum_str,
             exclusiveMinimum_str;

extern VALUE maxLength_str,
             minLength_str,
             pattern_str,
             format_str;

extern VALUE items_str,
             additionalItems_str,
             unevaluatedItems_str,
             contains_str,
             maxItems_str,
             minItems_str,
             uniqueItems_str,
             maxContains_str,
             minContains_str;

extern VALUE properties_str,
             patternProperties_str,
             additionalProperties_str,
             unevaluatedProperties_str,
             maxProperties_str,
             minProperties_str,
             required_str,
             dependentRequired_str;

void Init_keywords();
