# frozen_string_literal: true

RSpec.describe FastJSON::Schema do
  describe "anyOf" do
    let(:ruby_schema) { { "anyOf" => any_of } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :any_of, :valid?) do
        1 | [{ "type" => "string" }]                          | false
        1 | [{ "type" => "string" }, { "type" => "boolean" }] | false
        1 | [{ "type" => "string" }, { "type" => "integer" }] | true
        1 | [{ "type" => "integer" }]                         | true
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
