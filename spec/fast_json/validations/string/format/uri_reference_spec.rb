# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "format uri-reference" do
    it_behaves_like "string format", "uri-reference", -> {
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        ""                         | true
        "http://example.com"       | true
        "https://example.com/path" | true
        "/path"                    | true
        "path"                     | true
        "./path"                   | true
        "../path"                  | true
        "path/to/resource"         | true
        "?query=1"                 | true
        "#fragment"                | true
        "path?q=1#f"               | true
        "/p%20ath"                 | true
        "//ç.com/path"             | false
        "path with space"          | false
        "path%ZZ"                  | false
        "#%"                       | false
        "ç/path"                   | false
      end
    }
  end
end
