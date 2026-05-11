# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "format uri-template" do
    it_behaves_like "string format", "uri-template", -> {
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        ""                             | true
        "http://example.com/"          | true
        "http://example.com/{user}"    | true
        "/users/{user_id}"             | true
        "/files{/path*}"               | true
        "{?q,lang}"                    | true
        "{#section}"                   | true
        "{+reserved}"                  | true
        "{.label}"                     | true
        "{/segment}"                   | true
        "{;param}"                     | true
        "{&continued}"                 | true
        "{var:1}"                      | true
        "{var:99}"                     | true
        "{var:9999}"                   | true
        "{var*}"                       | true
        "{var1,var2}"                  | true
        "{+var1,var2*}"                | true
        "{?q,lang*,page:3}"            | true
        "{a.b}"                        | true
        "{a.b.c}"                      | true
        "{var_name}"                   | true
        "{var%20name}"                 | true
        "http://example.com/{path%2F}" | true
        "ç/{name}"                     | true
        "{=reserved}"                  | true
        "{!reserved}"                  | true
        "{@reserved}"                  | true
        "{|reserved}"                  | true
        "{,reserved}"                  | true
        "{"                            | false
        "{var"                         | false
        "var}"                         | false
        "http://example.com/{path"     | false
        "http://example.com/path}"     | false
        "{}"                           | false
        "{ var}"                       | false
        "{var }"                       | false
        "{var name}"                   | false
        "{var,}"                       | false
        "{var.}"                       | false
        "{var..name}"                  | false
        "{var-name}"                   | false
        "{var:0}"                      | false
        "{var:00}"                     | false
        "{var:10000}"                  | false
        "{var:-1}"                     | false
        "{var:*}"                      | false
        "{var**}"                      | false
        "{var*1}"                      | false
        "http://example.com/foo bar"   | false
        "http://example.com/foo<bar"   | false
        "http://example.com/foo%2"     | false
        "http://example.com/foo%ZZ"    | false
        "u\xFFser".dup.force_encoding("BINARY")         | false
        "u\xC0\xAFser".dup.force_encoding("BINARY")     | false # overlong
        "user\xED\xA0\x80".dup.force_encoding("BINARY") | false # surrogate
      end
    }
  end
end
