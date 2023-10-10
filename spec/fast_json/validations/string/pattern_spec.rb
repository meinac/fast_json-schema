# frozen_string_literal: true

RSpec.describe "FastJSON::Schema String" do
  describe "pattern" do
    let(:ruby_schema) { { "type" => "string", "pattern" => pattern } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :pattern, :valid?) do
        "Foo" | "\\A\\s+\\w+\\z" | false
        "Foo" | "bar"            | false
        "Foo" | "foo"            | false
        "Foo" | "Foo"            | true
        "Foo" | "\\A\\w+\\z"     | true
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
