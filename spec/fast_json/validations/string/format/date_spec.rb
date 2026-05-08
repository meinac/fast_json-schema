# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "format date" do
    it_behaves_like "string format", "date", -> {
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        "2020-01-01"           | true
        "1999-12-31"           | true
        "2024-02-29"           | true
        "2000-02-29"           | true
        "2023-02-29"           | false
        "1900-02-29"           | false
        "2020-13-01"           | false
        "2020-00-15"           | false
        "2020-01-32"           | false
        "2020-01-00"           | false
        "2020-1-1"             | false
        "2020/01/01"           | false
        ""                     | false
        "2020-01-01T00:00:00Z" | false
      end
    }
  end
end
