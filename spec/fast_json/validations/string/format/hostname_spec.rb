# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "format hostname" do
    it_behaves_like "string format", "hostname", -> {
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        "example.com"                  | true
        "sub.example.com"              | true
        "a"                            | true
        "a-b"                          | true
        "a-b-c"                        | true
        "123"                          | true
        "123abc"                       | true
        "abc123"                       | true
        "xn--n3h.example.com"          | true
        "example.com."                 | true
        "localhost"                    | true
        "#{"a" * 63}"                  | true # 63-octet label boundary
        "#{"a" * 63}.example.com"      | true # 63-octet leading label
        "#{(["a"] * 126).join(".")}.b" | true # 253-character total: 126*2 + 1 = 25 
        "-foo"                         | false # starts with hyphen
        "foo-"                         | false # ends with hyphen
        "foo.-bar.com"                 | false # interior label starts with hyphen
        "foo.bar-.com"                 | false # interior label ends with hyphen
        ".foo"                         | false # leading dot
        "foo..bar"                     | false # empty label
        "foo.bar.."                    | false # double trailing dot
        "#{"a" * 64}"                  | false # 64-octet label (over limit)
        "#{"a" * 64}.example.com"      | false # 64-octet leading label
        "#{"a" * 254}"                 | false # 254 chars (over total limit)
        "foo_bar.com"                  | false # underscore not allowed
        "foo bar.com"                  | false # space
        "foo!.com"                     | false # invalid character
        "ç.example.com"                | false # non-ASCII rejected for hostname
        "foo.com/"                     | false # slash
      end
    }
  end
end
