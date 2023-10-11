# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Array" do
  describe "minItems" do
    let(:ruby_schema) { { "type" => "array", "minItems" => min_items } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :min_items, :valid?) do
        [1, 2] | 3 | false
        [1, 2] | 2 | true
        [1, 2] | 1 | true
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
