# frozen_string_literal: true

require "stress_spec_helper"

RSpec.describe "FastJSON::Schema path GC compaction safety" do
  context "when GC.compact runs mid-validation via the error block" do
    let(:ruby_schema) { { "type" => "array", "items" => { "type" => "integer" } } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }
    let(:data) { ["foo", "bar"] }

    it "does not crash and yields well-formed paths when GC compacts between yielded errors" do
      errors = schema.validate(data).map { |error| force_compaction && error }

      expect(errors.length).to eq(2)
    end
  end

  context "when GC.compact runs between repeated deep-schema validations" do
    let(:depth) { 30 }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }
    let(:ruby_schema) do
      depth.times.inject({ "type" => "integer" }) do |inner, _|
        { "type" => "object", "properties" => { "x" => inner } }
      end
    end

    let(:bad_data) do
      depth.times.inject("not an integer") { |inner, _| { "x" => inner } }
    end

    it "validates repeatedly across GC.compact cycles without crashing" do
      50.times do
        errors = schema.validate(bad_data).to_a

        expect(errors).not_to be_empty

        force_compaction
      end
    end
  end

  context "when validation runs under GC.stress with auto_compact" do
    let(:schema) { FastJSON::Schema.create(ruby_schema) }
    let(:ruby_schema) do
      {
        "type" => "object",
        "properties" => {
          "list" => {
            "type" => "array",
            "items" => {
              "type" => "object",
              "properties" => { "n" => { "type" => "integer" } }
            }
          }
        }
      }
    end

    let(:data) do
      { "list" => Array.new(20) { |i| { "n" => (i.even? ? i : "bad-#{i}".dup) } } }
    end

    it "validates without crashing under GC.stress + auto_compact", :gc_stress, :gc_auto_compact do
      errors = schema.validate(data).to_a

      expect(errors.length).to eq(10)
    end
  end
end
