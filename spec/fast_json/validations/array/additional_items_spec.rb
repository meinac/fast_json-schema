# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Array" do
  describe "additionalItems" do
    let(:ruby_schema) { { "type" => "array", "items" => items, "additionalItems" => additional_items }.compact }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :items, :additional_items, :valid?) do
        [1, 2]    | nil                       | { "type" => "boolean" } | true
        [1, 2]    | { "type" => "integer" }   | { "type" => "boolean" } | true
        [1]       | [{ "type" => "integer" }] | { "type" => "boolean" } | true
        [1, true] | [{ "type" => "integer" }] | { "type" => "boolean" } | true
        [1, 2]    | [{ "type" => "integer" }] | { "type" => "boolean" } | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
