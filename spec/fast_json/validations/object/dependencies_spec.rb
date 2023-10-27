# frozen_string_literal: true

RSpec.describe "FastJSON::Schema Object" do
  describe "dependencies" do
    let(:ruby_schema) { { "type" => "object", "dependencies" => dependencies } }
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :dependencies, :valid?) do
        { "foo" => 1 }               | {}                          | true
        { "foo" => 1 }               | { "bar" => false }          | true
        { "foo" => 1 }               | { "foo" => ["foo"] }        | true
        { "foo" => 1, "bar" => nil } | { "foo" => ["foo", "bar"] } | true
        { "foo" => 1 }               | { "foo" => true }           | true
        { "foo" => 1 }               | { "foo" => {} }             | true
        { "foo" => 1 }               | { "foo" => false }          | false
        { "foo" => 1 }               | { "foo" => ["bar"] }        | false
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
