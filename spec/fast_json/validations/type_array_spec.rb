# frozen_string_literal: true

RSpec.describe FastJSON::Schema do
  describe "type as an array" do
    using RSpec::Parameterized::TableSyntax

    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "string or null" do
      let(:ruby_schema) { { "type" => ["string", "null"] } }

      where(:data, :valid?) do
        "foo" | true
        nil   | true
        1     | false
        1.1   | false
        true  | false
        []    | false
        {}    | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end

    describe "integer or string" do
      let(:ruby_schema) { { "type" => ["integer", "string"] } }

      where(:data, :valid?) do
        1      | true
        "foo"  | true
        1.1    | false
        {}     | false
        []     | false
        true   | false
        nil    | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end

    describe "array, object, or null" do
      let(:ruby_schema) { { "type" => ["array", "object", "null"] } }

      where(:data, :valid?) do
        [1, 2, 3]       | true
        { "foo" => 1 }  | true
        nil             | true
        123             | false
        "foo"           | false
        true            | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end

    describe "single-element array" do
      let(:ruby_schema) { { "type" => ["string"] } }

      where(:data, :valid?) do
        "foo" | true
        123   | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end

    describe "number accepts integer instances" do
      let(:ruby_schema) { { "type" => ["number"] } }

      it "accepts an integer" do
        expect(schema.valid?(1)).to be(true)
      end

      it "accepts a float" do
        expect(schema.valid?(1.5)).to be(true)
      end

      it "rejects a string" do
        expect(schema.valid?("1")).to be(false)
      end
    end

    describe "applying type-specific keywords alongside an array type" do
      let(:ruby_schema) do
        { "type" => ["string", "null"], "minLength" => 2 }
      end

      it "applies minLength to strings" do
        expect(schema.valid?("ab")).to be(true)
        expect(schema.valid?("a")).to be(false)
      end

      it "accepts null regardless of minLength" do
        expect(schema.valid?(nil)).to be(true)
      end
    end

    describe "compile-time errors" do
      it "raises for an empty array" do
        expect { FastJSON::Schema.create({ "type" => [] }) }
          .to raise_error(RuntimeError, /must contain at least one element/)
      end

      it "raises for a non-string element" do
        expect { FastJSON::Schema.create({ "type" => [1] }) }
          .to raise_error(RuntimeError, /elements must be strings/)
      end

      it "raises for an unknown type name" do
        expect { FastJSON::Schema.create({ "type" => ["foobar"] }) }
          .to raise_error(RuntimeError, /unknown type foobar/)
      end

      it "raises for duplicate entries" do
        expect { FastJSON::Schema.create({ "type" => ["string", "string"] }) }
          .to raise_error(RuntimeError, /duplicate type string/)
      end
    end
  end
end
