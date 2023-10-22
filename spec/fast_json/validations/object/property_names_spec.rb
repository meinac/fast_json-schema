# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Object" do
  describe "propertyNames" do
    let(:ruby_schema) { { "type" => "object", "propertyNames" => property_names } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :property_names, :valid?) do
        { "foo" => 1, "bar" => 2 } | true                 | true
        { "foo" => 1, "bar" => 2 } | {}                   | true
        { "foo" => 1, "bar" => 2 } | { "minLength" => 2 } | true
        { "foo" => 1, "bar" => 2 } | false                | false
        { "foo" => 1, "bar" => 2 } | { "minLength" => 4 } | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
