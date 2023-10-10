# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "maxLength" do
    let(:ruby_schema) { { "type" => "string", "maxLength" => max_length } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :max_length, :valid?) do
        "Foo" | 2 | false
        "Foo" | 3 | true
        "Foo" | 4 | true
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
