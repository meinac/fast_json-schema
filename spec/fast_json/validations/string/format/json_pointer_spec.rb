# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "format json-pointer" do
    it_behaves_like "string format", "json-pointer", -> {
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        ""                 | true # whole document
        "/"                | true # single empty token
        "/foo"             | true
        "/foo/bar"         | true
        "/foo/0"           | true
        "/foo/-"           | true # array '-' token
        "/a~1b"            | true # ~1 escape for '/'
        "/m~0n"            | true # ~0 escape for '~'
        "/~0/~1"           | true # multiple escapes
        "//"               | true # two empty tokens
        "/ç"               | true # UTF-8 in token
        "/key with spaces" | true
        '/key"with"quotes' | true
        "foo"              | false
        "/~"               | false
        "/~2"              | false
        "/~a"              | false
        "/foo~"            | false
        "/foo~/bar"        | false
        "u\xFFser".dup.force_encoding("BINARY")          | false
        "/u\xC0\xAFser".dup.force_encoding("BINARY")     | false # overlong
        "/user\xED\xA0\x80".dup.force_encoding("BINARY") | false # surrogate
      end
    }
  end
end
