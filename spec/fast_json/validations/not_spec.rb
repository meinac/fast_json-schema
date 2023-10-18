# frozen_string_literal: true

RSpec.describe FastJSON::Schema do
  describe "not" do
    let(:ruby_schema) { { "not" => not_schema } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :not_schema, :valid?) do
        1 | false | true
        1 | true  | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
