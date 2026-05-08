# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "format time" do
    it_behaves_like "string format", "time", -> {
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        "00:00:00Z"             | true
        "23:59:59Z"             | true
        "23:59:60Z"             | true
        "12:34:56.789+02:00"    | true
        "12:34:56-05:30"        | true
        "12:34:56.123456789Z"   | true
        "00:00:00+00:00"        | true
        "24:00:00Z"             | false
        "12:60:00Z"             | false
        "12:34:61Z"             | false
        "12:34:56"              | false
        "12:34:56z"             | false
        "12:34"                 | false
        "12:34:56+5:00"         | false
        "12:34:56."             | false
        "12:34:56.+02:00"       | false
        "12:34:56+24:00"        | false
        "12:34:56+02:60"        | false
        ""                      | false
      end
    }
  end
end
