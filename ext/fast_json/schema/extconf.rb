require "mkmf"

$srcs = Dir.glob("#{__dir__}/**/*.c")
$VPATH << "$(srcdir)/formats"

create_makefile("fast_json/schema/ext/schema")
