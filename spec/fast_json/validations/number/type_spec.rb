# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Number" do
  describe "type" do
    let(:ruby_schema) { { "type" => "number" } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :valid?) do
        []     | false
        true   | false
        false  | false
        1      | true
        nil    | false
        1.2    | true
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
