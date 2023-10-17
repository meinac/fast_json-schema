# frozen_string_literal: true

RSpec.describe FastJSON::Schema do
  describe "const" do
    let(:ruby_schema) { { "const" => const } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :const, :valid?) do
        0 | 2 | false
        1 | 2 | false
        2 | 2 | true
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
