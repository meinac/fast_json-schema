# frozen_string_literal: true

RSpec.describe FastJSON::Schema do
  describe "enum" do
    let(:ruby_schema) { { "enum" => enum } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :enum, :valid?) do
        0 | [2, 3] | false
        1 | [2, 3] | false
        2 | [2, 3] | true
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
