# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Object" do
  describe "minProperties" do
    let(:ruby_schema) { { "type" => "object", "minProperties" => min_properties } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :min_properties, :valid?) do
        { "foo" => 1, "bar" => 2 } | 3 | false
        { "foo" => 1, "bar" => 2 } | 2 | true
        { "foo" => 1, "bar" => 2 } | 1 | true
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
