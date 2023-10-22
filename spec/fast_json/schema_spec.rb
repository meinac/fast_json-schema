# frozen_string_literal: true

RSpec.describe FastJSON::Schema do
  it "has a version number" do
    expect(FastJSON::Schema::VERSION).not_to be nil
  end

  describe "#validate" do
    let(:data) { [1] }
    let(:ruby_schema) { { "type" => "array", "items" => { "type" => "integer" } } }
    let(:schema) { described_class.create(ruby_schema) }

    subject(:validate) { schema.validate(data, &block) }

    context "when there is not a block given" do
      let(:block) { nil }

      it { is_expected.to be_an_instance_of(Enumerator) }

      context "when it is chained with another enumerator method" do
        subject(:error_count) { validate.map(&:itself).length }

        context "when there is no error" do
          it { is_expected.to be_zero }
        end

        context "when there are errors" do
          let(:data) { [1, false, true] }

          it { is_expected.to be(2) }
        end
      end
    end

    context "when there is a block given" do
      let(:errors) { [] }
      let(:block) { Proc.new { |error| errors << error } }

      before do
        validate
      end

      context "when there is no error" do
        it "does not yield any error to give block" do
          expect(errors.length).to be_zero
        end
      end

      context "when there are errors" do
        let(:data) { [1, false, true] }

        it "yields erros to given block" do
          expect(errors.length).to be(2)
        end
      end
    end
  end

  describe "#valid?" do
    let(:ruby_schema) { { "type" => "array", "items" => { "type" => "integer" } } }
    let(:schema) { described_class.create(ruby_schema) }

    subject { schema.valid?(data) }

    context "where there is no error" do
      let(:data) { [1] }

      it { is_expected.to be(true) }
    end

    context "when there is an error" do
      let(:data) { ["text"] }

      it { is_expected.to be(false) }
    end
  end
end
