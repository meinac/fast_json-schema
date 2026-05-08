#include "date.h"
#include "error.h"

#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9')

/*
* Days in each month of the year.
* February has 28 days in a common year and 29 days in a leap year(See `days_in_month` function for more details).
*/
static const int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static bool is_leap_year(int year) {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static int days_in_month(int year, int month) {
  if(month == 2 && is_leap_year(year)) return 29;

  return days[month - 1];
}

/*
* Strict RFC 3339 full-date parser: YYYY-MM-DD
* Returns true if valid, false otherwise.
*/
bool parse_full_date(const char *s, long len) {
  if(len != 10) return false;

  if(!IS_DIGIT(s[0]) || !IS_DIGIT(s[1]) || !IS_DIGIT(s[2]) || !IS_DIGIT(s[3])) return false;
  if(s[4] != '-') return false;
  if(!IS_DIGIT(s[5]) || !IS_DIGIT(s[6])) return false;
  if(s[7] != '-') return false;
  if(!IS_DIGIT(s[8]) || !IS_DIGIT(s[9])) return false;

  int year  = (s[0] - '0') * 1000 + (s[1] - '0') * 100 + (s[2] - '0') * 10 + (s[3] - '0');
  int month = (s[5] - '0') * 10 + (s[6] - '0');
  int day   = (s[8] - '0') * 10 + (s[9] - '0');

  if(month < 1 || month > 12) return false;
  if(day < 1 || day > days_in_month(year, month)) return false;

  return true;
}

void validate_format_date(VALUE schema, CompiledSchema *compiled_schema, VALUE data, Context *context) {
  if(!parse_full_date(RSTRING_PTR(data), RSTRING_LEN(data)))
    yield_error(compiled_schema, data, context, "format_date");
}
