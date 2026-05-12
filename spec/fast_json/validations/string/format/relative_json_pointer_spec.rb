# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "format relative-json-pointer" do
    it_behaves_like "string format", "relative-json-pointer", -> {
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        "0"         | true 
        "1"         | true
        "10"        | true
        "123"       | true
        "0/foo"     | true
        "2/foo/bar" | true
        "0#"        | true
        "5#"        | true
        "1/a~1b"    | true 
        "0/ç"       | true # UTF-8 in pointer portion
        ""          | false # must have integer prefix
        "01"        | false # leading zero
        "00"        | false # leading zero
        "-1"        | false # negative
        "#"         | false # no integer prefix
        "1foo"      | false # after integer, must be '#' or '/'-pointer
        "1##"       | false # only one '#' allowed
        "0#/foo"    | false # '#' cannot be followed by anything
        "1/~"       | false # invalid pointer portion
        "1/~2"      | false # invalid escape
        "a/foo"     | false # non-digit at position 0
        "1/u\xFFser".dup.force_encoding("BINARY") | false # malformed UTF-8
      end
    }
  end
end
