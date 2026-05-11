# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "format idn-hostname" do
    it_behaves_like "string format", "idn-hostname", -> {
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        "example.com"         | true
        "sub.example.com"     | true
        "a"                   | true
        "a-b"                 | true
        "123"                 | true
        "xn--n3h.example.com" | true
        "example.com."        | true
        "ç.example.com"       | true
        "münchen.de"          | true
        "δοκιμή.gr"           | true
        "пример.рф"           | true
        "-foo"                | false
        "foo-"                | false
        "ç-.example.com"      | false
        ".foo"                | false
        "foo..bar"            | false
        "foo_bar.com"         | false
        "foo bar.com"         | false

        # Invalid - malformed UTF-8 byte sequences
        "u\xFFser.example.com".dup.force_encoding("BINARY")         | false
        "u\xC0\xAFser.example.com".dup.force_encoding("BINARY")     | false # overlong
        "user\xED\xA0\x80.example.com".dup.force_encoding("BINARY") | false # surrogate U+D800
      end
    }
  end
end
