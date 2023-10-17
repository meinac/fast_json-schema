# frozen_string_literal: true

RSpec.describe FastJSON::Schema do
  describe "if" do
    let(:data) { 4 }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    subject { schema.valid?(data) }

    context "when the data is invalid against `if` schema" do
      let(:ruby_schema) { { "if" => { "type" => "string" }, **rest } }

      context "when the `else` schema is not provided" do
        let(:rest) { {} }

        it { is_expected.to be_truthy }
      end

      context "when the `else` schema is provided" do
        context "when the data is not valid against `else` schema" do
          let(:rest) { { "else" => { "type" => "integer", "maximum" => 3 } } }

          it { is_expected.to be_falsey }
        end

        context "when the data is valid against `else` schema" do
          let(:rest) { { "else" => { "type" => "integer", "maximum" => 4 } } }

          it { is_expected.to be_truthy }
        end
      end
    end

    context "when the data is valid against `if` schema" do
      let(:ruby_schema) { { "if" => { "type" => "integer" }, **rest } }

      context "when the `then` schema is not provided" do
        let(:rest) { {} }

        it { is_expected.to be_truthy }
      end

      context "when the `then` schema is provided" do
        context "when the data is not valid against `then` schema" do
          let(:rest) { { "then" => { "type" => "integer", "maximum" => 3 } } }

          it { is_expected.to be_falsey }
        end

        context "when the data is valid against `then` schema" do
          let(:rest) { { "then" => { "type" => "integer", "maximum" => 4 } } }

          it { is_expected.to be_truthy }
        end
      end
    end
  end
end
