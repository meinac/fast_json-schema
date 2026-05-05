# frozen_string_literal: true

RSpec.describe "FastJSON::Schema path GC safety" do
  context "with a deeply nested schema" do
    let(:ruby_schema) do
      {
        "type" => "object",
        "properties" => {
          "a" => {
            "type" => "object",
            "properties" => {
              "b" => {
                "type" => "object",
                "properties" => {
                  "c" => { "type" => "integer" }
                }
              }
            }
          }
        }
      }
    end

    it "compiles a deeply nested schema without crashing under GC.stress", :gc_stress do
      expect { FastJSON::Schema.create(ruby_schema) }.not_to raise_error
    end
  end

  context "with a schema with collection keywords" do
    let(:ruby_schema) do
      {
        "allOf" => [
          { "type" => "integer", "minimum" => 0 },
          { "type" => "integer", "maximum" => 100 }
        ],
        "anyOf" => [
          { "type" => "integer" },
          { "type" => "string" }
        ],
        "items" => [
          { "type" => "integer" },
          { "type" => "string" },
          { "type" => "boolean" }
        ]
      }
    end

    it "compiles a schema with collection keywords without crashing under GC.stress", :gc_stress do
      expect { FastJSON::Schema.create(ruby_schema) }.not_to raise_error
    end
  end

  context "with a schema that validates an array" do
    let(:ruby_schema) { { "type" => "array", "items" => { "type" => "integer" } } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    it "validates and yields error paths without crashing under GC.stress", :gc_stress do
      expect { schema.validate([1, "a", 2, "b"]).to_a }.not_to raise_error
    end
  end

  context "with a schema that validates an object" do
    let(:ruby_schema) { { "type" => "object", "properties" => { "name" => { "type" => "string" } } } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    it "validates and yields error paths without crashing under GC.stress", :gc_stress do
      expect { schema.validate("name" => 42).to_a }.not_to raise_error
    end
  end
end
