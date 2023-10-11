# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Array" do
  describe "maxContains" do
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      context "when there is no `contains` keyword" do
        let(:ruby_schema) { { "type" => "array", "maxContains" => max_contains } }

        where(:data, :max_contains, :valid?) do
          [1, 2]    | 1 | true
          [1, true] | 1 | true
        end

        with_them do
          subject { schema.valid?(data) }

          it { is_expected.to be(valid?) }
        end
      end

      context "when there is `contains` keyword" do
        let(:ruby_schema) { { "type" => "array", "contains" => { "type" => "integer" }, "maxContains" => max_contains } }

        where(:data, :max_contains, :valid?) do
          [1, 2]    | 1 | false
          [1, true] | 1 | true
        end

        with_them do
          subject { schema.valid?(data) }

          it { is_expected.to be(valid?) }
        end
      end
    end
  end
end
