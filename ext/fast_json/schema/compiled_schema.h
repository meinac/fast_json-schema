#include "types/compiled_schema.h"

extern VALUE compiled_schema_class;
extern const rb_data_type_t compiled_schema_type;

#define GetCompiledSchema(compiled_schema_obj, compiled_schema) TypedData_Get_Struct((compiled_schema_obj), CompiledSchema, &compiled_schema_type, (compiled_schema))

void compile_schema(VALUE);
void Init_compiled_schema(VALUE);
