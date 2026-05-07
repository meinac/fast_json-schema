# frozen_string_literal: true

RSpec.describe FastJSON::Schema do
  describe "$ref" do
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "local ref to a sibling property" do
      using RSpec::Parameterized::TableSyntax

      let(:ruby_schema) do
        {
          "type" => "object",
          "properties" => {
            "name"     => { "type" => "string", "minLength" => 1 },
            "nickname" => { "$ref" => "#/properties/name" }
          }
        }
      end

      where(:data, :valid?) do
        { "name" => "alice", "nickname" => "ally" } | true
        { "name" => "alice", "nickname" => "" }     | false
        { "name" => "alice", "nickname" => 5 }      | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end

    describe "$ref ignores sibling keywords (Draft 7)" do
      using RSpec::Parameterized::TableSyntax

      let(:ruby_schema) do
        {
          "type" => "object",
          "properties" => {
            "name"  => { "type" => "string", "minLength" => 1 },
            "alias" => { "$ref" => "#/properties/name", "type" => "integer" }
          }
        }
      end

      where(:data, :valid?) do
        { "name" => "a", "alias" => "b" } | true
        { "name" => "a", "alias" => 5 }   | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end

    describe "recursive self-ref" do
      using RSpec::Parameterized::TableSyntax

      let(:ruby_schema) do
        {
          "type" => "object",
          "properties" => {
            "value" => { "type" => "integer" },
            "next"  => { "$ref" => "#" }
          },
          "required" => ["value"]
        }
      end

      where(:data, :valid?) do
        { "value" => 1, "next" => { "value" => 2, "next" => { "value" => 3 } } }     | true
        { "value" => 1, "next" => { "value" => 2, "next" => { "value" => "bad" } } } | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
