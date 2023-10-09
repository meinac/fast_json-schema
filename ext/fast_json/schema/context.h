#include "types/context.h"

extern VALUE context_class;
extern const rb_data_type_t context_type;

#define GetContext(context_object, context) TypedData_Get_Struct((context_object), Context, &context_type, (context))

VALUE create_context_object(VALUE);
void Init_context(VALUE);
