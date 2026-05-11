# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "format uri" do
    it_behaves_like "string format", "uri", -> {
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        "http://example.com"                        | true
        "https://example.com/path/to/resource"      | true
        "ftp://user:pass@example.com:21/path"       | true
        "urn:isbn:0451450523"                       | true
        "mailto:alice@example.com"                  | true
        "file:///etc/hosts"                         | true
        "http://192.168.0.1/foo"                    | true
        "http://[2001:db8::1]:8080/foo"             | true
        "http://[v1.fe80::a+en1]/"                  | true
        "http://example.com/path?query=1&q2=2#frag" | true
        "http://example.com/p%20ath"                | true
        "a:b"                                       | true
        "my+app.scheme://x"                         | true
        "http://example.com/"                       | true
        "http://example.com/?"                      | true
        "http://example.com/?q"                     | true
        "http://example.com/#"                      | true
        ""                                          | false
        "example.com"                               | false
        "/foo/bar"                                  | false
        "1http://x"                                 | false
        "http//x"                                   | false
        "http://example.com/foo bar"                | false
        "http://example.com/foo%2"                  | false
        "http://example.com/foo%ZZ"                 | false
        "http://[invalid]/"                         | false
        "http://ç.com"                              | false
      end
    }
  end
end
