#include <ruby.h>
#include "types/compile_context.h"

CompileContext compile_context_descend(const CompileContext *parent, VALUE step);
