# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "unknown format" do
    it_behaves_like "string format", "unknown", -> {
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        "anything" | true
        "1234"     | true
      end
    }
  end
end
