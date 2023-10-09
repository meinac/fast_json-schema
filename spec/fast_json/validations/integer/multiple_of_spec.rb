# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Integer" do
  describe "multipleOf" do
    let(:ruby_schema) { { "type" => "integer", "multipleOf" => multiple_of } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :multiple_of, :valid?) do
        4 | 2   | true
        4 | 3   | false
        1 | 0.2 | true
        1 | 0.3 | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
