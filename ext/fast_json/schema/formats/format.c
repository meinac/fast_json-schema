#include "format.h"
#include "date.h"
#include "time.h"
#include "date_time.h"
#include "email.h"
#include "idn_email.h"

void no_op_format_validate(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  return;
}

format_validation_function format_validation_function_for(VALUE format_val) {
  if(!RB_TYPE_P(format_val, T_STRING)) return no_op_format_validate;

  const char *format_str = StringValueCStr(format_val);

  if(strcmp(format_str, "date") == 0)      return validate_format_date;
  if(strcmp(format_str, "time") == 0)      return validate_format_time;
  if(strcmp(format_str, "date-time") == 0) return validate_format_date_time;
  if(strcmp(format_str, "email") == 0)     return validate_format_email;
  if(strcmp(format_str, "idn-email") == 0) return validate_format_idn_email;

  return no_op_format_validate;
}
