# frozen_string_literal: true

require "fast_json/schema"
require "rspec-parameterized"

RSpec.configure do |config|
  # Enable flags like --only-failures and --next-failure
  config.example_status_persistence_file_path = ".rspec_status"

  # Disable RSpec exposing methods globally on `Module` and `main`
  config.disable_monkey_patching!

  config.expect_with :rspec do |c|
    c.syntax = :expect
  end

  config.around(:each, :gc_stress) do |example|
    previous = GC.stress
    GC.stress = true

    example.run
  ensure
    GC.stress = previous
  end
end
