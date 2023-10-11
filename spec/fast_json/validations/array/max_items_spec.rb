# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Array" do
  describe "maxItems" do
    let(:ruby_schema) { { "type" => "array", "maxItems" => max_items } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :max_items, :valid?) do
        [1, 2] | 1 | false
        [1, 2] | 2 | true
        [1, 2] | 3 | true
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
