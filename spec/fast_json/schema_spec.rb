# frozen_string_literal: true

RSpec.describe FastJSON::Schema do
  it "has a version number" do
    expect(FastJSON::Schema::VERSION).not_to be nil
  end
end
