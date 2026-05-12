# frozen_string_literal: true

RSpec.describe "FastJSON::Schema custom format attributes" do
  describe "valid?" do
    let(:validator) { ->(s) { true } }
    let(:formats) { { "starts-with-x" => validator } }
    let(:ruby_schema) { { "format" => "starts-with-x" } }
    let(:schema) { FastJSON::Schema.create(ruby_schema, formats: formats) }

    context "when the validator returns true" do
      let(:validator) { ->(s) { s.is_a?(String) && s.start_with?("X") } }

      it "accepts matching data" do
        expect(schema.valid?("Xfoo")).to be(true)
      end

      it "rejects non-matching data" do
        expect(schema.valid?("nope")).to be(false)
      end

      it "skips the custom format for non-string data (per JSON Schema)" do
        expect(schema.valid?(42)).to be(true)
      end
    end

    context "truthy/falsy coercion" do
      context "when the validator returns nil" do
        let(:validator) { ->(_) { nil } }

        it "treats nil as invalid" do
          expect(schema.valid?("anything")).to be(false)
        end
      end

      context "when the validator returns false" do
        let(:validator) { ->(_) { false } }

        it "treats false as invalid" do
          expect(schema.valid?("anything")).to be(false)
        end
      end

      context "when the validator returns an integer" do
        let(:validator) { ->(_) { 42 } }

        it "treats integer as valid (truthy)" do
          expect(schema.valid?("anything")).to be(true)
        end
      end

      context "when the validator returns an empty string" do
        let(:validator) { ->(_) { "" } }

        it "treats empty string as valid (truthy in Ruby)" do
          expect(schema.valid?("anything")).to be(true)
        end
      end
    end

    context "when the validator raises" do
      let(:validator) { ->(_) { raise "boom" } }

      it "is caught and treated as invalid" do
        expect(schema.valid?("anything")).to be(false)
      end
    end

    context "overriding a built-in format" do
      let(:formats) { { "date" => ->(_) { true } } }
      let(:ruby_schema) { { "format" => "date" } }

      it "user-supplied wins over the built-in date validator" do
        expect(schema.valid?("not-a-real-date")).to be(true)
      end
    end

    context "unknown format with no custom registered" do
      let(:ruby_schema) { { "format" => "totally-unknown" } }

      it "accepts any value (no-op)" do
        expect(schema.valid?("anything")).to be(true)
      end
    end

    context "nested schema using a custom format" do
      let(:validator) { ->(s) { s.is_a?(String) && s.start_with?("X") } }
      let(:ruby_schema) { { "type" => "object", "properties" => { "field" => { "format" => "starts-with-x" } } } }

      context "when the validator returns true" do
        it "accepts matching data" do
          expect(schema.valid?({ "field" => "Xyz" })).to be(true)
        end
      end
      
      context "when the validator returns false" do
        it "rejects non-matching data" do
          expect(schema.valid?({ "field" => "nope" })).to be(false)
        end
      end
    end

    context "custom format inside items" do
      let(:validator) { ->(s) { s.is_a?(String) && s.start_with?("X") } }
      let(:ruby_schema) { { "type" => "array", "items" => { "format" => "starts-with-x" } } }

      context "when the validator returns true" do
        it "accepts matching data" do
          expect(schema.valid?(["Xa", "Xb"])).to be(true)
        end
      end

      context "when the validator returns false" do
        it "rejects non-matching data" do
          expect(schema.valid?(["Xa", "nope"])).to be(false)
        end
      end
    end

    context "error key uses the format name verbatim" do
      let(:formats) { { "Hello-World" => ->(_) { false } } }
      let(:ruby_schema) { { "format" => "Hello-World" } }

      it "yields format_<verbatim-name>" do
        errors = []
        schema.validate("data") { |error| errors << error }

        expect(errors.size).to eq(1)
        expect(errors.first.type).to eq("format_Hello-World")
      end
    end
  end

  describe "argument validation" do
    let(:ruby_schema) { { "format" => "x" } }
    let(:schema) { FastJSON::Schema.create(ruby_schema, formats: formats) }

    context "when formats is nil" do
      let(:formats) { nil }

      it "accepts nil" do
        expect { schema }.not_to raise_error
      end
    end

    context "when formats is neither nil nor a Hash" do
      let(:formats) { [] }

      it "raises TypeError" do
        expect { schema }.to raise_error(TypeError, /must be a Hash/)
      end
    end

    context "when a format name is not a String" do
      let(:formats) { { 42 => ->(_) { true } } }

      it "raises TypeError" do
        expect { schema }.to raise_error(TypeError, /format name must be a String/)
      end
    end

    context "when a value does not respond to :call" do
      let(:formats) { { "x" => 42 } }

      it "raises TypeError" do
        expect { schema }.to raise_error(TypeError, /must respond to :call/)
      end
    end
  end
end
