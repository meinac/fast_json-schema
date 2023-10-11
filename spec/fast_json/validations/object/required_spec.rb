# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Object" do
  describe "required" do
    let(:ruby_schema) { { "type" => "object", "required" => required } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :required, :valid?) do
        { "foo" => 1, "bar" => 2 } | ["foo"]        | true
        { "foo" => 1, "bar" => 2 } | ["foo", "bar"] | true
        { "foo" => 1, "bar" => 2 } | ["zoo"]        | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
