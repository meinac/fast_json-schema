# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Integer" do
  describe "exclusiveMaximum" do
    let(:ruby_schema) { { "type" => "integer", "exclusiveMaximum" => exclusive_maximum } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :exclusive_maximum, :valid?) do
        4 | 2   | false
        4 | 4   | false
        4 | 5   | true
        4 | 3.9 | false
        4 | 4.0 | false
        4 | 4.1 | true
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
