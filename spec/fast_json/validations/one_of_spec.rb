# frozen_string_literal: true

RSpec.describe FastJSON::Schema do
  describe "oneOf" do
    let(:ruby_schema) { { "oneOf" => one_of } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :one_of, :valid?) do
        4 | [{ "type" => "string" }]                                              | false
        4 | [{ "type" => "integer" }, { "type" => "integer", "multipleOf" => 2 }] | false
        4 | [{ "type" => "integer" }]                                             | true
        4 | [{ "type" => "string" }, { "type" => "integer", "multipleOf" => 2 }]  | true
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
