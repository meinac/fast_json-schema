#include "validate_number.h"
#include "error.h"

void validate_number(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!RB_INTEGER_TYPE_P(data) && !RB_TYPE_P(data, T_FLOAT))
    return yield_error(compiled_schema, data, context, "type_number");

  double data_c;

  if(RB_INTEGER_TYPE_P(data)) {
    data_c = NUM2DBL(data);
  } else {
    data_c = NUM2LONG(data);
  }


  if(RB_INTEGER_TYPE_P(compiled_schema->multipleOf_val)) {
    long multipleOf = NUM2LONG(compiled_schema->multipleOf_val);
    double div = data_c / multipleOf;

    if(div - (int)div != 0.0) // fmod() does not work here due to IEEE floating point arithmetics
      yield_error(compiled_schema, data, context, "multipleOf");
  } else if(RB_TYPE_P(compiled_schema->multipleOf_val, T_FLOAT)) {
    double multipleOf = NUM2DBL(compiled_schema->multipleOf_val);
    double div = data_c / multipleOf;

    if(div - (int)div != 0.0) // fmod() does not work here due to IEEE floating point arithmetics
      yield_error(compiled_schema, data, context, "multipleOf");
  }

  if(RB_INTEGER_TYPE_P(compiled_schema->maximum_val)) {
    if(data_c > NUM2LONG(compiled_schema->maximum_val))
      yield_error(compiled_schema, data, context, "maximum");
  } else if(RB_TYPE_P(compiled_schema->maximum_val, T_FLOAT)) {
    if(data_c > NUM2DBL(compiled_schema->maximum_val))
      yield_error(compiled_schema, data, context, "maximum");
  }

  if(RB_INTEGER_TYPE_P(compiled_schema->exclusiveMaximum_val)) {
    if(data_c >= NUM2LONG(compiled_schema->exclusiveMaximum_val))
      yield_error(compiled_schema, data, context, "exclusiveMaximum");
  } else if(RB_TYPE_P(compiled_schema->exclusiveMaximum_val, T_FLOAT)) {
    if(data_c >= NUM2DBL(compiled_schema->exclusiveMaximum_val))
      yield_error(compiled_schema, data, context, "exclusiveMaximum");
  }

  if(RB_INTEGER_TYPE_P(compiled_schema->minimum_val)) {
    if(data_c < NUM2LONG(compiled_schema->minimum_val))
      yield_error(compiled_schema, data, context, "minimum");
  } else if(RB_TYPE_P(compiled_schema->minimum_val, T_FLOAT)) {
    if(data_c < NUM2DBL(compiled_schema->minimum_val))
      yield_error(compiled_schema, data, context, "minimum");
  }

  if(RB_INTEGER_TYPE_P(compiled_schema->exclusiveMinimum_val)) {
    if(data_c <= NUM2LONG(compiled_schema->exclusiveMinimum_val))
      yield_error(compiled_schema, data, context, "exclusiveMinimum");
  } else if(RB_TYPE_P(compiled_schema->exclusiveMinimum_val, T_FLOAT)) {
    if(data_c <= NUM2DBL(compiled_schema->exclusiveMinimum_val))
      yield_error(compiled_schema, data, context, "exclusiveMinimum");
  }
}
