# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Object" do
  describe "additionalProperties" do
    let(:ruby_schema) do
      {
        "type" => "object",
        "properties" => properties,
        "patternProperties" => pattern_properties,
        "additionalProperties" => additional_properties
      }
    end

    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :properties, :pattern_properties, :additional_properties, :valid?) do
        { "foo" => 1 } | {}                                   | {}                                  | {}                      | true
        { "foo" => 1 } | { "foo" => { "type" => "integer" } } | {}                                  | { "type" => "string" }  | true
        { "foo" => 1 } | {}                                   | { "fo" => { "type" => "integer" } } | { "type" => "string" }  | true
        { "foo" => 1 } | { "foo" => { "type" => "integer" } } | {}                                  | false                   | true
        { "foo" => 1 } | {}                                   | { "fo" => { "type" => "integer" } } | false                   | true
        { "foo" => 1 } | {}                                   | {}                                  | {}                      | true
        { "foo" => 1 } | {}                                   | {}                                  | true                    | true
        { "foo" => 1 } | {}                                   | {}                                  | { "type" => "integer" } | true
        { "foo" => 1 } | {}                                   | {}                                  | false                   | false
        { "foo" => 1 } | {}                                   | {}                                  | { "type" => "string" }  | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
