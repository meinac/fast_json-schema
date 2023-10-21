# frozen_string_literal: true

RSpec.describe FastJSON::Schema do
  describe "validating instance by data type when the `type` keyword is missing" do
    let(:schema) { FastJSON::Schema.create(ruby_schema) }

    describe "valid?" do
      using RSpec::Parameterized::TableSyntax

      where(:data, :ruby_schema, :valid?) do
        0                   | { 'minimum' => 1 }       | false
        1                   | { 'minimum' => 1 }       | true
        1.0                 | { 'multipleOf' => 2 }    | false
        2.0                 | { 'multipleOf' => 2 }    | true
        ''                  | { 'minLength' => 1 }     | false
        'a'                 | { 'minLength' => 1 }     | true
        {}                  | { 'minProperties' => 1 } | false
        { 'foo' => 'bar' }  | { 'minProperties' => 1 } | true
        []                  | { 'minItems' => 1 }      | false
        [1]                 | { 'minItems' => 1 }      | true
      end

      with_them do
        subject { schema.valid?(data) }

        it { is_expected.to be(valid?) }
      end
    end
  end
end
