#include "compile_context.h"
#include "path.h"

/*
* Builds a child context by appending `step` to current_pointer when a base is active.
* `step` is a JSON-Pointer step string (e.g. "properties/foo", "allOf/0", "if").
*/
CompileContext compile_context_descend(const CompileContext *parent, VALUE step) {
    CompileContext child = *parent;

    if (parent->current_base != Qnil && parent->current_base != Qundef) {
      VALUE parent_pointer = parent->current_pointer;

      if (parent_pointer == Qnil || parent_pointer == Qundef) {
        parent_pointer = rb_str_new_cstr("");
      }

      child.current_pointer = new_path(parent_pointer, step);
    }

    return child;
  }