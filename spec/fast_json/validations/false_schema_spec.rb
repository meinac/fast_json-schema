# frozen_string_literal: true

RSpec.describe FastJSON::Schema do
  let(:ruby_schema) { false }
  let(:schema) { described_class.create(ruby_schema) }

  describe "valid?" do
    subject { schema.valid?({}) }

    it { is_expected.to be_falsey }
  end
end
