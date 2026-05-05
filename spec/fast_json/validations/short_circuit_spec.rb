# frozen_string_literal: true

RSpec.describe FastJSON::Schema do
  let(:schema) { described_class.create(ruby_schema) }
  let(:data) { { "tagged" => { "kind" => "c" }, "name" => "foo" } }
  let(:ruby_schema) do
    {
      "type" => "object",
      "properties" => {
        "tagged" => {
          "anyOf" => [
            { "type" => "object", "properties" => { "kind" => { "const" => "a" }, "value" => { "type" => "integer" } }, "required" => ["kind"] },
            { "type" => "object", "properties" => { "kind" => { "const" => "b" }, "value" => { "type" => "string"  } }, "required" => ["kind"] }
          ]
        },
        "name" => { "type" => "string" }
      }
    }
  end

  subject(:validate) do 
    schema.validate(data) do |_error|
      raise "BOOM!"
    end
  end

  it "propagates user-block exceptions out of validate cleanly" do
    expect { validate }.to raise_error(RuntimeError, "BOOM!")
  end
end
