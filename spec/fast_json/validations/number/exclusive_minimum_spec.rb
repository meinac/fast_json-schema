# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Number" do
  describe "exclusiveMinimum" do
    let(:ruby_schema) { { "type" => "number", "exclusiveMinimum" => exclusive_minimum } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :exclusive_minimum, :valid?) do
        4.0 | 5   | false
        4.0 | 5.0 | false
        4.0 | 4   | false
        4.0 | 4.0 | false
        4.0 | 4.1 | false
        4   | 5   | false
        4   | 5.0 | false
        4   | 4   | false
        4   | 4.0 | false
        4   | 4.1 | false
        4.0 | 3   | true
        4.0 | 3.9 | true
        4   | 3   | true
        4   | 3.9 | true
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
