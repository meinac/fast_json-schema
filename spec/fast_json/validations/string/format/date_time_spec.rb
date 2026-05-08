# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "format date-time" do
    it_behaves_like "string format", "date-time", -> {
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        "2020-01-01T00:00:00Z"           | true
        "2024-02-29T23:59:60+00:00"      | true
        "2020-01-01T12:34:56.123-05:00"  | true
        "1999-12-31T23:59:59Z"           | true
        "2020-01-01 00:00:00Z"           | false
        "2020-01-01t00:00:00Z"           | false
        "2020-01-01"                     | false
        "2023-02-29T00:00:00Z"           | false
        "T00:00:00Z"                     | false
        "2020-01-01T"                    | false
        "2020-01-01T25:00:00Z"           | false
        ""                               | false
      end
    }
  end
end
