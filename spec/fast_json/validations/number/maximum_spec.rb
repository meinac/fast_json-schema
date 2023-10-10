# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Number" do
  describe "maximum" do
    let(:ruby_schema) { { "type" => "number", "maximum" => maximum } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :maximum, :valid?) do
        4.0 | 2   | false
        4.0 | 2.0 | false
        4   | 2   | false
        4   | 2.0 | false
        4.0 | 5   | true
        4.0 | 5.1 | true
        4.0 | 4   | true
        4.0 | 4.0 | true
        4   | 5   | true
        4   | 5.1 | true
        4   | 4   | true
        4   | 4.0 | true
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
