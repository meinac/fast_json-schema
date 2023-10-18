# frozen_string_literal: true

RSpec.describe FastJSON::Schema do
  describe "allOf" do
    let(:ruby_schema) { { "allOf" => all_of } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :all_of, :valid?) do
        4 | [{ "type" => "string" }]                                              | false
        4 | [{ "type" => "integer" }, { "type" => "integer", "multipleOf" => 3 }] | false
        4 | [{ "type" => "integer" }]                                             | true
        4 | [{ "type" => "integer" }, { "type" => "integer", "multipleOf" => 2 }] | true
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
