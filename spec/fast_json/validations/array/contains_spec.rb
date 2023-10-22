# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Array" do
  describe "contains" do
    let(:ruby_schema) { { "type" => "array", "contains" => contains } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :contains, :valid?) do
        [1, 2]    | { "type" => "integer" } | true
        [1, true] | { "type" => "boolean" } | true
        [1, true] | true                    | true
        [1, true] | {}                      | true
        [1, true] | false                   | false
        [1, 1]    | { "type" => "boolean" } | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
