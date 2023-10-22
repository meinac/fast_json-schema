# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Array" do
  describe "items" do
    let(:ruby_schema) { { "type" => "array", "items" => items } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :items, :valid?) do
        [1, 2]    | true                                              | true
        [1, 2]    | {}                                                | true
        [1, 2]    | { "type" => "integer" }                           | true
        [1, true] | [{ "type" => "integer" }]                         | true
        [1, nil]  | [{ "type" => "integer" }, true]                   | true
        [1, nil]  | [{ "type" => "integer" }, {}]                     | true
        [1, nil]  | [{ "type" => "integer" }, false]                  | false
        [1, nil]  | [{ "type" => "integer" }, { "type" => "string" }] | false
        [1, 2]    | { "type" => "boolean" }                           | false
        [1, 2]    | false                                             | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
