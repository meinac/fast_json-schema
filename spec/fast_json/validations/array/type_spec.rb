# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Array" do
  describe "type" do
    let(:ruby_schema) { { "type" => "array" } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        []     | true
        true   | false
        false  | false
        1      | false
        nil    | false
        1.2    | false
        {}     | false
        "test" | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
