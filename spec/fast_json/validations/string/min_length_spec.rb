# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "minLength" do
    let(:ruby_schema) { { "type" => "string", "minLength" => min_length } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :min_length, :valid?) do
        "Foo" | 4 | false
        "Foo" | 3 | true
        "Foo" | 2 | true
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
