# frozen_string_literal: true

RSpec.describe FastJSON::Schema do
  describe "$ref" do
    using RSpec::Parameterized::TableSyntax

    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    subject { schema.valid?(data) }

    describe "local ref to a sibling property" do
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
        it { is_expected.to be(valid?) }
      end
    end

    describe "$ref ignores sibling keywords (Draft 7)" do
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
        it { is_expected.to be(valid?) }
      end
    end

    describe "recursive self-ref" do
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
        it { is_expected.to be(valid?) }
      end
    end

    describe "$ref into #/definitions" do
      let(:ruby_schema) do
        {
          "$ref" => "#/definitions/Address",
          "definitions" => {
            "Address" => { "type" => "string", "minLength" => 1 }
          }
        }
      end

      where(:data, :valid?) do
        "home" | true
        ""     | false
        5      | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "$ref into #/$defs" do
      let(:ruby_schema) do
        {
          "$ref" => "#/$defs/Email",
          "$defs" => {
            "Email" => { "type" => "string", "minLength" => 3 }
          }
        }
      end

      where(:data, :valid?) do
        "ab@"  | true
        "a"    | false
        42     | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "$ref into nested custom container" do
      let(:ruby_schema) do
        {
          "$ref" => "#/components/schemas/User",
          "components" => {
            "schemas" => {
              "User" => {
                "type" => "object",
                "required" => ["id"],
                "properties" => {
                  "id" => { "type" => "integer" }
                }
              }
            }
          }
        }
      end

      where(:data, :valid?) do
        { "id" => 1 }   | true
        { "id" => "x" } | false
        {}              | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "$ref into known keyword on the same node" do
      let(:ruby_schema) do
        {
          "$ref" => "#/properties/name",
          "properties" => {
            "name" => { "type" => "string", "minLength" => 1 }
          }
        }
      end

      where(:data, :valid?) do
        "alice" | true
        ""      | false
        5       | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "cross-reference between definitions" do
      let(:ruby_schema) do
        {
          "type" => "object",
          "properties" => {
            "value" => { "$ref" => "#/definitions/A" }
          },
          "definitions" => {
            "A" => { "$ref" => "#/definitions/B" },
            "B" => { "type" => "integer" }
          }
        }
      end

      where(:data, :valid?) do
        { "value" => 1 }     | true
        { "value" => "bad" } | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "recursive self-reference inside a definition" do
      let(:ruby_schema) do
        {
          "$ref" => "#/definitions/Node",
          "definitions" => {
            "Node" => {
              "type" => "object",
              "properties" => {
                "value" => { "type" => "integer" },
                "next"  => { "$ref" => "#/definitions/Node" }
              },
              "required" => ["value"]
            }
          }
        }
      end

      where(:data, :valid?) do
        { "value" => 1, "next" => { "value" => 2, "next" => { "value" => 3 } } }     | true
        { "value" => 1, "next" => { "value" => 2, "next" => { "value" => "bad" } } } | false
        { "next" => { "value" => 1 } }                                               | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    describe "unresolved $ref into a missing definition" do
      let(:ruby_schema) do
        {
          "$ref" => "#/definitions/Missing",
          "definitions" => {
            "Other" => { "type" => "string" }
          }
        }
      end

      it "raises a RuntimeError when compiled" do
        expect { schema }.to raise_error(RuntimeError, /Unresolved \$ref/)
      end
    end
  end
end
