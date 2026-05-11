# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "format iri-reference" do
    it_behaves_like "string format", "iri-reference", -> {
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        ""                                      | true
        "http://example.com"                    | true
        "http://ç.com/path"                     | true
        "/ç"                                    | true
        "ç"                                     | true
        "./ç"                                   | true
        "?q=ç"                                  | true
        "#ç"                                    | true
        "//ç.com/path"                          | true
        "path with space"                       | false
        "path%ZZ"                               | false
        "u\xFFser".dup.force_encoding("BINARY") | false
      end
    }
  end
end
