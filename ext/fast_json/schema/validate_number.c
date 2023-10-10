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
}
