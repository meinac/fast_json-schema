# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Object" do
  describe "patternProperties" do
    let(:ruby_schema) { { "type" => "object", "patternProperties" => pattern_properties } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :pattern_properties, :valid?) do
        { "foo" => 1 }  | {}                                   | true
        { "foo" => 1 }  | { "bar" => false }                   | true
        { "foo" => 1 }  | { "foo" => true }                    | true
        { "foo" => 1 }  | { "foo" => {} }                      | true
        { "foo" => 1 }  | { "fo" => {} }                       | true
        { "foo" => 1 }  | { "foo" => { "type" => "integer" } } | true
        { "foo" => 1 }  | { "foo" => false }                   | false
        { "foo" => 1 }  | { "fo" => false }                    | false
        { "foo" => 1 }  | { "foo" => { "type" => "string" } }  | false
        { "foo" => 1 }  | { "fo" => { "type" => "string" } }   | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
