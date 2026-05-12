#include "format.h"
#include "date.h"
#include "time.h"
#include "date_time.h"
#include "email.h"
#include "idn_email.h"
#include "hostname.h"
#include "idn_hostname.h"
#include "ipv4.h"
#include "ipv6.h"
#include "uri.h"
#include "uri_reference.h"
#include "iri.h"
#include "iri_reference.h"
#include "uri_template.h"
#include "regex.h"
#include "json_pointer.h"
#include "relative_json_pointer.h"

void no_op_format_validate(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  return;
}

format_validation_function format_validation_function_for(VALUE format_val) {
  if(!RB_TYPE_P(format_val, T_STRING)) return no_op_format_validate;

  const char *format_str = StringValueCStr(format_val);

  if(strcmp(format_str, "date") == 0)                  return validate_format_date;
  if(strcmp(format_str, "time") == 0)                  return validate_format_time;
  if(strcmp(format_str, "date-time") == 0)             return validate_format_date_time;
  if(strcmp(format_str, "email") == 0)                 return validate_format_email;
  if(strcmp(format_str, "idn-email") == 0)             return validate_format_idn_email;
  if(strcmp(format_str, "hostname") == 0)              return validate_format_hostname;
  if(strcmp(format_str, "idn-hostname") == 0)          return validate_format_idn_hostname;
  if(strcmp(format_str, "ipv4") == 0)                  return validate_format_ipv4;
  if(strcmp(format_str, "ipv6") == 0)                  return validate_format_ipv6;
  if(strcmp(format_str, "uri") == 0)                   return validate_format_uri;
  if(strcmp(format_str, "uri-reference") == 0)         return validate_format_uri_reference;
  if(strcmp(format_str, "iri") == 0)                   return validate_format_iri;
  if(strcmp(format_str, "iri-reference") == 0)         return validate_format_iri_reference;
  if(strcmp(format_str, "uri-template") == 0)          return validate_format_uri_template;
  if(strcmp(format_str, "regex") == 0)                 return validate_format_regex;
  if(strcmp(format_str, "json-pointer") == 0)          return validate_format_json_pointer;
  if(strcmp(format_str, "relative-json-pointer") == 0) return validate_format_relative_json_pointer;

  return no_op_format_validate;
}
