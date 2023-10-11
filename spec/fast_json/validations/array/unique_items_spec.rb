# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Array" do
  describe "uniqueItems" do
    let(:ruby_schema) { { "type" => "array", "uniqueItems" => unique_items } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :unique_items, :valid?) do
        [1, 2] | false | true
        [1, 1] | false | true
        [1, 2] | true  | true
        [1, 1] | true  | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
