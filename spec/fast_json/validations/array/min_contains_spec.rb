# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Array" do
  describe "minContains" do
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      context "when there is no `contains` keyword" do
        let(:ruby_schema) { { "type" => "array", "minContains" => min_contains } }

        where(:data, :min_contains, :valid?) do
          [1, 2]    | 1 | true
          [1, true] | 1 | true
          []        | 1 | true
        end

        with_them do
          subject { schema.valid?(data) }

          it { is_expected.to be(valid?) }
        end
      end

      context "when there is `contains` keyword" do
        let(:ruby_schema) { { "type" => "array", "contains" => { "type" => "integer" }, "minContains" => min_contains } }

        where(:data, :min_contains, :valid?) do
          [true, "foo"] | 0 | true
          [1, true]     | 1 | true
          [1, true]     | 2 | false
        end

        with_them do
          subject { schema.valid?(data) }

          it { is_expected.to be(valid?) }
        end
      end
    end
  end
end
