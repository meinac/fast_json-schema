# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Integer" do
  describe "minimum" do
    let(:ruby_schema) { { "type" => "integer", "minimum" => minimum } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :minimum, :valid?) do
        4 | 2   | true
        4 | 4   | true
        4 | 5   | false
        4 | 3.9 | true
        4 | 4.0 | true
        4 | 4.1 | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
