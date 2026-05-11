# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "format email" do
    it_behaves_like "string format", "email", -> {
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        # Valid - dot-atom local-parts
        "alice@example.com"               | true
        "user.name+tag@example.co.uk"     | true
        "x@x.io"                          | true
        "!#$%&'*+-/=?^_`{|}~@example.com" | true
        "a@localhost"                     | true
        "a@example.com."                  | true

        # Valid - quoted-string local-parts
        '"quoted local"@example.com'    | true
        '"escaped\\"quote"@example.com' | true
        '"with space"@example.com'      | true

        # Valid - domain-literals
        "user@[192.168.0.1]"                | true
        "user@[0.0.0.0]"                    | true
        "user@[255.255.255.255]"            | true
        "user@[IPv6:2001:db8::1]"           | true
        "user@[IPv6:2001:db8:0:0:0:0:0:1]"  | true
        "user@[IPv6:::1]"                   | true
        "user@[IPv6:::]"                    | true
        "user@[IPv6:2001:db8::192.168.0.1]" | true
        "user@[my-tag:abc123]"              | true

        # Valid - 64-octet local-part boundary
        "#{"a" * 64}@example.com" | true

        # Invalid - structural
        ""                  | false
        "plainaddress"      | false
        "@example.com"      | false
        "user@"             | false
        "user@@example.com" | false

        # Invalid - dot rules
        ".user@example.com"  | false
        "user.@example.com"  | false
        "us..er@example.com" | false
        "user@.example.com"  | false
        "user@example..com"  | false

        # Invalid - whitespace and CFWS
        "user name@example.com"     | false
        "user(comment)@example.com" | false
        " user@example.com"         | false
        "user@example.com "         | false

        # Invalid - IPv4 literal
        "user@[256.0.0.1]" | false
        "user@[1.2.3]"     | false
        "user@[1.2.3.4.5]" | false
        "user@[1.2.3.4"    | false

        # Invalid - IPv6 literal
        "user@[IPv6:gggg::1]"           | false
        "user@[IPv6:1:2:3:4:5:6:7:8:9]" | false
        "user@[IPv6:1::2::3]"           | false
        "user@[2001:db8::1]"            | false # missing IPv6: tag

        # Invalid - General-address-literal
        "user@[unknown]" | false
        "user@[:abc]"    | false
        "user@[tag:]"    | false

        # Invalid - length boundaries
        "#{"a" * 65}@example.com" | false # local-part > 64
        "user@#{"a" * 256}"       | false # domain > 255

        # Invalid - non-ASCII rejected for plain email
        "用户@example.com" | false
      end
    }
  end
end
