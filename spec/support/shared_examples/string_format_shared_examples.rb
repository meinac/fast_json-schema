# frozen_string_literal: true

RSpec.shared_examples "string format" do |format, test_data|
  let(:ruby_schema) { { "type" => "string", "format" => format } }
  let(:schema) { FastJSON::Schema.create(ruby_schema) }

  describe "valid?" do
    using RSpec::Parameterized::TableSyntax

    subject { schema.valid?(data) }

    context "when the data is a string" do
      instance_exec(&test_data)

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    context "when the data is not a string" do
      where(:data, :valid?) do
        [42, true, false, nil, { "foo" => "bar" }] | false
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end

    context "when the format is not for string" do
      let(:ruby_schema) { { "format" => "date" } }

      where(:data, :valid?) do
        [42, true, false, nil, { "foo" => "bar" }] | true
      end

      with_them do
        it { is_expected.to be(valid?) }
      end
    end
  end
end