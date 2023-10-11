# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Object" do
  describe "maxProperties" do
    let(:ruby_schema) { { "type" => "object", "maxProperties" => max_properties } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :max_properties, :valid?) do
        { "foo" => 1, "bar" => 2 } | 3 | true
        { "foo" => 1, "bar" => 2 } | 2 | true
        { "foo" => 1, "bar" => 2 } | 1 | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
