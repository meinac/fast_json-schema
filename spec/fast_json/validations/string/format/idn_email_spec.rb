# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "format idn-email" do
    it_behaves_like "string format", "idn-email", -> {
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        # Valid - ASCII forms still accepted
        "alice@example.com"           | true
        "user.name+tag@example.co.uk" | true
        "a@localhost"                 | true
        "a@example.com."              | true
        "user@[192.168.0.1]"          | true
        "user@[IPv6:2001:db8::1]"     | true
        "user@[my-tag:abc123]"        | true

        # Valid - non-ASCII (UTF-8) forms
        "用户@例子.广告"             | true
        "δοκιμή@παράδειγμα.δοκιμή" | true
        "Pelé@example.com"         | true
        "user@münchen.de"          | true
        '"用户名"@example.com'      | true
        "user@[my-tag:λabc]"       | true

        # Invalid - same structural failures as email
        ""                          | false
        "plainaddress"              | false
        "@example.com"              | false
        "user@"                     | false
        "user@@example.com"         | false
        ".user@example.com"         | false
        "user.@example.com"         | false
        "us..er@example.com"        | false
        "user@.example.com"         | false
        "user@example..com"         | false
        "user name@example.com"     | false
        "user(comment)@example.com" | false

        # Invalid - bad IPv4/IPv6 literals (still ASCII-only)
        "user@[256.0.0.1]"    | false
        "user@[IPv6:gggg::1]" | false
        "user@[IPv6:1::2::3]" | false

        # Invalid - non-ASCII inside IPv4 literal (only General-address-literal allows UTF-8)
        "用户@[192.168.0.\xC3\xA1]".dup.force_encoding("UTF-8") | false

        # Invalid - malformed UTF-8 sequences
        "u\xFFser@example.com".dup.force_encoding("BINARY")         | false
        "u\xC0\xAFser@example.com".dup.force_encoding("BINARY")     | false # overlong
        "user\xED\xA0\x80@example.com".dup.force_encoding("BINARY") | false # surrogate U+D800
        "user@example.com\xF5".dup.force_encoding("BINARY")         | false # codepoint > U+10FFFF lead byte
      end
    }
  end
end
