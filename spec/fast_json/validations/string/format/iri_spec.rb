# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "format iri" do
    it_behaves_like "string format", "iri", -> {
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        "http://example.com"                | true
        "https://example.com/path?q=1#f"    | true
        "urn:isbn:0451450523"               | true
        "http://ç.com/path"                 | true
        "http://example.com/?q=ç"           | true
        "http://example.com/#ç"             | true
        "http://münchen.de/straße"          | true
        ""                                  | false
        "ç.com"                             | false
        "/ç"                                | false
        "http://example.com/u\xFFser".dup.force_encoding("BINARY")      | false
        "http://example.com/u\xC0\xAFser".dup.force_encoding("BINARY")  | false
        "http://example.com/u\xED\xA0\x80".dup.force_encoding("BINARY") | false
      end
    }
  end
end
