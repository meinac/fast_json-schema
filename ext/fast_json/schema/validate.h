#include <ruby.h>
#include "compiled_schema.h"
#include "if.h"
#include "not.h"
#include "validate_array.h"
#include "validate_bool.h"
#include "validate_integer.h"
#include "validate_null.h"
#include "validate_number.h"
#include "validate_object.h"
#include "validate_string.h"

void no_op_validate(VALUE, CompiledSchema *, VALUE, Context *);
void false_validate(VALUE, CompiledSchema *, VALUE, Context *);
void validate(VALUE, CompiledSchema *, VALUE, Context *);
